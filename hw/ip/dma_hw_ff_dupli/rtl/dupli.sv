module dupli (
    input logic clk_i,
    input logic rst_ni,
    // Interfaz hadware fifo
    output hw_fifo_req_done,
    input dma_fifo_pkg::fifo_req_t hw_fifo_req_i,
    output dma_fifo_pkg::fifo_resp_t hw_fifo_resp_o

);

  localparam RW_FIFO_DEPTH_W = 16;
  logic [4 : 0] conta;
  logic         done;
  logic         Flag_state;
  logic [ 16:0] conta1;

  logic [ 15:0] datoin_dup;
  logic         start_dup;
  logic         valid_r_dup;
  logic         valid_w_dup;
  logic         done_dup;
  logic         ready_r_dup;
  logic         ready_w_dup;
  logic [ 15:0] datout_dup;


  always_ff @(posedge clk_i or negedge rst_ni) begin
    if (!rst_ni) begin
      Flag_state <= 1'b0;
    end else begin
      if (hw_fifo_req_i.push) begin
        Flag_state <= 1'b1;
      end else if (done_dup) begin
        Flag_state <= 1'b0;
        //$display("--Ciclos totales--: %d", conta1);
      end

      if (Flag_state) begin
        conta1 <= conta1 + 1;
        if (conta1 == 65530) begin
          conta1 <= 0;
        end
      end
    end
  end


  DupliTop u_DupliTop (
      .rst    (~rst_ni),
      .clk    (clk_i),
      .start  (start_dup),
      .done   (done_dup),
      .ready_r(ready_r_dup),
      .valid_r(valid_r_dup),
      .datoin (datoin_dup),
      .ready_w(ready_w_dup),
      .valid_w(valid_w_dup),
      .datout (datout_dup)
  );

  assign valid_r_dup = ~hw_r_fifo_empty;
  assign datoin_dup  = hw_r_fifo_data_out;
  assign ready_w_dup = ~hw_w_fifo_full | hw_w_fifo_empty;
  assign start_dup   = Flag_state;

  // --- 1. FIFO DE ENTRADA (DMA -> HW) --- 
  logic hw_r_fifo_full, hw_r_fifo_empty, hw_r_fifo_pop;  //Creo senales intermedias.
  logic [15:0] hw_r_fifo_data_out;
  fifo_v3 #(
      .DEPTH(RW_FIFO_DEPTH_W),
      .FALL_THROUGH(1'b1),
      .DATA_WIDTH(16)
  ) hw_r_fifo_i (
      .clk_i(clk_i),
      .rst_ni,
      .flush_i(1'b0),
      .testmode_i(1'b0),
      .full_o(hw_r_fifo_full),
      .empty_o(hw_r_fifo_empty),
      .usage_o(),
      .data_i(hw_fifo_req_i.data),  //X
      .push_i(hw_fifo_req_i.push),  //X
      .data_o(hw_r_fifo_data_out),
      .pop_i(hw_r_fifo_pop)
  );
  assign hw_fifo_resp_o.full     = hw_r_fifo_full;
  assign hw_fifo_resp_o.alm_full = hw_r_fifo_full;
  assign hw_r_fifo_pop           = ready_r_dup;

  // --- 2. FIFO DE SALIDA (HW -> DMA) --- 
  logic hw_w_fifo_full, hw_w_fifo_empty, hw_w_fifo_push;
  logic [15:0] hw_w_fifo_data_in;
  fifo_v3 #(
      .DEPTH(RW_FIFO_DEPTH_W),
      .FALL_THROUGH(1'b0),
      .DATA_WIDTH(16)
  ) hw_w_fifo_i (
      .clk_i(clk_i),
      .rst_ni,
      .flush_i(1'b0),
      .testmode_i(1'b0),
      .full_o(hw_w_fifo_full),
      .empty_o(hw_w_fifo_empty),
      .usage_o(),
      .data_i(hw_w_fifo_data_in),
      .push_i(hw_w_fifo_push),
      .data_o(hw_fifo_resp_o.data),
      .pop_i(hw_fifo_req_i.pop)
  );

  assign hw_fifo_resp_o.empty    = hw_w_fifo_empty;
  assign hw_w_fifo_data_in = datout_dup;
  assign hw_w_fifo_push = valid_w_dup;


  logic [11:0] count_q;
  always_ff @(posedge clk_i or negedge rst_ni) begin
    if (!rst_ni) begin
      count_q <= '0;
      done <= 1'b0;
      //end else if (hw_fifo_req_i.pop) begin
    end else begin
      if (hw_fifo_req_i.push && count_q == 0) begin
        done <= 1'b0;
      end
      if (hw_fifo_req_i.pop) begin
        count_q <= count_q + 1;
        if (count_q == 1919) begin
          count_q <= '0;  // Reseteamos el contador
          done    <= 1'b1;  // ¡Levantamos la bandera de DONE!
          $display("[%t] DONE ENVIADO AL DMA", $time);
        end
      end

      //count_q <= count_q + 1;
      //if (count_q == 15) begin
      //  count_q <= '0;
      //  $display("[%t]Done", $time);
      //end
    end
    //if (hw_fifo_req_i.push) begin  //aqui depuramos
    //  $display("[%t] DMA IN: %d - DUP IN: %d", $time, hw_fifo_req_i.data, datoin_dup);
    //end
    //if (hw_fifo_req_i.pop) begin
    //  $display("[%t] DMA OUT: %d", $time, hw_fifo_resp_o.data);
    //end
    //if (ready_w_dup && valid_w_dup) begin
    //  $display("[%t] DUP OUT: %d", $time, datout_dup);
    //end
    //if (ready_r_dup && valid_r_dup) begin
    //  $display("[%t] DUP IN: %d", $time, datoin_dup);
    //end
  end
  assign hw_fifo_req_done = done;

endmodule
