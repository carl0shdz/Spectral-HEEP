module Pro_Sub (
    input logic clk_i,
    input logic rst_ni,
    // Interfaz hadware fifo
    output hw_fifo_req_done,
    input Pro_Sub_pkg::fifo_req_t hw_fifo_req_i,
    output Pro_Sub_pkg::fifo_resp_t hw_fifo_resp_o

);

  localparam RW_FIFO_DEPTH_W = 256;
  logic [4 : 0] conta;
  logic         done;
  logic         Flag_state;
  logic [ 16:0] conta1;

  logic [ 31:0] datoin_pro;
  logic [ 31:0] datoin_sub;
  logic [ 31:0] datoin_sub_r;
  logic         start_pro;


  logic         valid_r_pro;
  logic         valid_r_sub;
  logic         valid_r_sub_r;
  logic         valid_w_pro;
  logic         valid_w_pro_x;
  logic         valid_w_pro_p;
  logic         valid_w_sub;
  logic         done_pro;
  logic         done_sub;


  logic         ready_r_pro;
  logic         ready_r_sub;
  logic         ready_r_sub_r;
  logic         ready_w_pro;
  logic         ready_w_pro_x;
  logic         ready_w_pro_p;
  logic         ready_w_sub;
  logic [ 31:0] datout_pro;
  logic [ 31:0] datout_pro_x;
  logic [ 31:0] datout_pro_p;
  logic [ 31:0] datout_sub;


  always_ff @(posedge clk_i or negedge rst_ni) begin
    if (!rst_ni) begin
      Flag_state <= 1'b0;
    end else begin
      if (hw_fifo_req_i.push) begin
        Flag_state <= 1'b1;
        //$display("[%t] START", $time);
      end else if (done) begin
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


  ProjectionPixelTop u_ProjectionPixelTop (
      .rst(~rst_ni),
      .clk(clk_i),
      .start(start_pro),
      .done(done_pro),
      .ready_r(ready_r_pro),
      .valid_r(valid_r_pro),
      .datoin(datoin_pro),
      .ready_w_pro(ready_w_pro_p),
      .valid_w_pro(valid_w_pro_p),
      .datout_pro(datout_pro_p),
      .ready_w_x(ready_w_pro_x),
      .valid_w_x(valid_w_pro_x),
      .datout_x(datout_pro_x),
      .ready_w(ready_w_pro),
      .valid_w(valid_w_pro),
      .datout(datout_pro)
  );

  SubtractionPixelTop u_SubtractionPixelTop (
      .rst(~rst_ni),
      .clk(clk_i),
      .start(start_pro),
      .done(done_sub),
      .ready_r(ready_r_sub),
      .valid_r(valid_r_sub),
      .datoin(datoin_sub),
      .ready_r_pr(ready_r_sub_r),
      .valid_r_pr(valid_r_sub_r),
      .datoin_pr(datoin_sub_r),
      .ready_w(ready_w_sub),
      .valid_w(valid_w_sub),
      .datout(datout_sub)
  );







  assign valid_r_pro = ~hw_r_fifo_empty;
  assign datoin_pro  = hw_r_fifo_data_out;
  assign ready_w_sub = ~hw_w_fifo_full | hw_w_fifo_empty;
  assign start_pro   = Flag_state;

  // --- 1. FIFO DE ENTRADA (DMA -> HW) --- 
  logic hw_r_fifo_full, hw_r_fifo_empty, hw_r_fifo_pop;  //Creo senales intermedias.
  logic [31:0] hw_r_fifo_data_out;
  fifo_v3 #(
      .DEPTH(RW_FIFO_DEPTH_W),
      .FALL_THROUGH(1'b1),
      .DATA_WIDTH(32)
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
  assign hw_r_fifo_pop           = ready_r_pro;

  // --- 2. FIFO DE SALIDA (HW -> DMA) --- 
  logic hw_w_fifo_full, hw_w_fifo_empty, hw_w_fifo_push;
  logic [31:0] hw_w_fifo_data_in;
  fifo_v3 #(
      .DEPTH(RW_FIFO_DEPTH_W),
      .FALL_THROUGH(1'b0),
      .DATA_WIDTH(32)
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

  assign hw_fifo_resp_o.empty = hw_w_fifo_empty;
  assign hw_w_fifo_data_in = datout_sub;
  assign hw_w_fifo_push = valid_w_sub;

  // --- 3. Stream (Projection -> Subtraction) --- 
  logic pro_fifo_full, pro_fifo_empty, pro_fifo_push, pro_fifo_pop;
  logic [31:0] pro_fifo_data_in;
  logic [31:0] pro_fifo_data_out;
  fifo_v3 #(
      .DEPTH(120),
      .FALL_THROUGH(1'b0),
      .DATA_WIDTH(32)
  ) pro_fifo_i (
      .clk_i(clk_i),
      .rst_ni,
      .flush_i(1'b0),
      .testmode_i(1'b0),
      .full_o(pro_fifo_full),
      .empty_o(pro_fifo_empty),
      .usage_o(),
      .data_i(pro_fifo_data_in),
      .push_i(pro_fifo_push),
      .data_o(pro_fifo_data_out),
      .pop_i(pro_fifo_pop)
  );
  //projection -> FIFO
  assign pro_fifo_data_in = datout_pro_p;
  assign pro_fifo_push = valid_w_pro_p;
  assign ready_w_pro_p = ~pro_fifo_full | pro_fifo_empty;
  assign ready_w_pro_x = 1'b1;
  //FIFO -> subtraction
  assign datoin_sub_r = pro_fifo_data_out;
  assign valid_r_sub_r = ~pro_fifo_empty;
  assign pro_fifo_pop = ready_r_sub_r;


  // --- 4. Stream (Payload -> Payload) --- 
  logic sub_fifo_full, sub_fifo_empty, sub_fifo_push, sub_fifo_pop;
  logic [31:0] sub_fifo_data_in;
  logic [31:0] sub_fifo_data_out;
  fifo_v3 #(
      .DEPTH(120),
      .FALL_THROUGH(1'b0),
      .DATA_WIDTH(32)
  ) sub_fifo_i (
      .clk_i(clk_i),
      .rst_ni,
      .flush_i(1'b0),
      .testmode_i(1'b0),
      .full_o(sub_fifo_full),
      .empty_o(sub_fifo_empty),
      .usage_o(),
      .data_i(sub_fifo_data_in),
      .push_i(sub_fifo_push),
      .data_o(sub_fifo_data_out),
      .pop_i(sub_fifo_pop)
  );
  //Projection -> FIFO
  assign sub_fifo_data_in = datout_pro;
  assign sub_fifo_push = valid_w_pro;
  assign ready_w_pro = ~sub_fifo_full | sub_fifo_empty;
  //FIFO -> subtraction
  assign datoin_sub = sub_fifo_data_out;
  assign valid_r_sub = ~sub_fifo_empty;
  assign sub_fifo_pop = ready_r_sub;





  logic [15:0] count_q;
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
        //$display("Conta Out: %d", count_q);
        //$display("[%t] RR", $time);
        if (count_q == 1919) begin  //1920 datos para un bloque de salida	
          count_q <= '0;  // Reseteamos el contador
          done    <= 1'b1;  // ¡Levantamos la bandera de DONE!
          $display("[%t] DONE ENVIADO AL DMA", $time);
        end
      end
    end
    //if (hw_fifo_req_i.push) begin  //aqui depuramos
    //  $display("DM_IN: %d", hw_fifo_req_i.data);
    //end
    //if (hw_r_fifo_pop) begin  //aqui depuramos
    //  $display("DM_IN: %d", datoin_pro);
    //end
    //if (hw_fifo_req_i.pop) begin
    //  $display("[%t] DMA OUT: %d", $time, hw_fifo_resp_o.data);
    //end
    //if (valid_w_pro && ready_w_pro) begin
    //  $display("PRO OUT: %d", sub_fifo_data_in);
    //end
    //if (ready_w_pro_p && valid_w_pro_p) begin               //datos a la salida de proyeccion y entrada de D3 
    //  $display("PROJ OUT: %d", datout_pro_p);
    //end
    //if (valid_r_sub_r && ready_r_sub_r) begin
    //  $display("D3 OUT: %d", datoin_sub_r);
    //end
    //if (pro_fifo_push) begin
    //  $display("D3 IN: %d", pro_fifo_data_in);
    //end
    //if (pro_fifo_pop) begin
    //  $display("D3 OUT: %d", pro_fifo_data_out);
    //end
  end
  assign hw_fifo_req_done = done;

endmodule
