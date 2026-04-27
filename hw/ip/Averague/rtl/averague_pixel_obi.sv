module averague_pixel_obi #(
    parameter int unsigned W = 16  // ancho de datos
) (
    input logic clk_i,
    input logic rst_ni,

    // Interfaz OBI entrada
    input  averague_obi_pkg::obi_req_t  obi_req_i,
    output averague_obi_pkg::obi_resp_t obi_rsp_o,

    // Interfaz OBI salida
    input  averague_obi_pkg::obi_req_t  obi_req_i_2,
    output averague_obi_pkg::obi_resp_t obi_rsp_o_2

    // Registros de control
    //input  averague_reg_pkg::reg_req_t  reg_req_i,
    //output averague_reg_pkg::reg_resp_t reg_rsp_o
);

  //control reg
  //logic start;
  logic        Flag_state;
  //logic done;
  //logic idle;

  //senales temporales

  logic        dummy_unused_be = |obi_req_i.be;
  // Señales para FIFO de entrada
  logic        fifo0_wr_en;
  logic        fifo0_rd_en;
  logic [15:0] fifo0_din;
  logic [15:0] fifo0_dout;
  logic        fifo0_full;
  logic        fifo0_empty;  //

  // Señales para FIFO 1
  logic        fifo1_wr_en;
  logic        fifo1_rd_en;
  logic [15:0] fifo1_din;
  logic [15:0] fifo1_dout;
  logic        fifo1_full;
  logic        fifo1_empty;


  logic [15:0] datoin_ave;
  logic        start_ave;
  logic        valid_r_ave;
  logic        valid_w_ave;
  logic        done_ave;
  logic        ready_r_ave;
  logic        ready_w_ave;
  logic [15:0] datout_ave;
  logic [16:0] conta;
  logic [16:0] conta1;

  // Instancia de FIFO_0 entrada de datos
  FIFO_V1 u_fifo_0 (
      .clk  (clk_i),
      .rst_n(rst_ni),
      .wr_en(fifo0_wr_en),  // Tienes que definir la lógica de esto
      .rd_en(fifo0_rd_en),  // Tienes que definir la lógica de esto
      .din  (fifo0_din),    // Tienes que definir la lógica de esto
      .dout (fifo0_dout),
      .full (fifo0_full),
      .empty(fifo0_empty)   //
  );

  /*fifo_generator_0 Fifo_In (
      .clk  (clk_i),        // Conectas tus señales de SV
      .srst (!rst_ni),      // Ojo: X-HEEP suele usar reset low, tu FIFO es reset high
      .din  (fifo0_din),
      .wr_en(fifo0_wr_en),
      .rd_en(fifo0_rd_en),
      .dout (fifo0_dout),
      .full (fifo0_full),
      .empty(fifo0_empty)
  );*/

  // Instancia de FIFO_1 salida de datos.
  FIFO_V2 u_fifo_1 (
      .clk  (clk_i),
      .rst_n(rst_ni),
      .wr_en(fifo1_wr_en),  // X
      .rd_en(fifo1_rd_en),  // X
      .din  (fifo1_din),    // X
      .dout (fifo1_dout),   // X
      .full (fifo1_full),   // X
      .empty(fifo1_empty)
  );

  // Registros de control
  //averague_control_reg u_ctrl (
  //    .clk_i  (clk_i),
  //    .rst_ni (rst_ni),
  //    .req_i  (reg_req_i),
  //    .rsp_o  (reg_rsp_o),
  //    .done_i (done_ave),
  //    .idle_i (),
  //    .start_o(start)
  //);

  AveragePixelTop u_AveragePixelTop (
      .rst    (~rst_ni),
      .clk    (clk_i),
      .start  (start_ave),
      .done   (done_ave),
      .ready_r(ready_r_ave),
      .valid_r(valid_r_ave),
      .datoin (datoin_ave),
      .ready_w(ready_w_ave),
      .valid_w(valid_w_ave),
      .datout (datout_ave)
  );


  //Regla #1  Start
  always_ff @(posedge clk_i or negedge rst_ni) begin
    if (!rst_ni) begin
      Flag_state <= 1'b0;
    end else begin
      if (Flag_state) begin
        conta1 <= conta1 + 1;
        if (conta1 == 65530) begin
          conta1 <= 0;
        end
      end

      if (obi_req_i.req) begin
        Flag_state <= 1'b1;
      end else if (done_ave) begin
        Flag_state <= 1'b0;
        $display("--Ciclos totales--: %d", conta1);
      end

    end
  end
  assign start_ave = Flag_state;



  //Regla #2 Canal Stream_In_OBI --> FIFO_0
  localparam OBI_ADDR_DATAIN = 32'hf000_0000;
  logic obi_gnt;
  logic obi_rvalid_q;
  logic [15:0] obi_rdata_q;
  assign obi_rdata_q = '0;
  assign obi_gnt = obi_req_i.req && (obi_req_i.addr == OBI_ADDR_DATAIN) && !fifo0_full;
  assign fifo0_wr_en = (obi_gnt && obi_req_i.we) ? 1'b1 : 1'b0;
  assign fifo0_din = obi_req_i.wdata[W-1:0];
  always_ff @(posedge clk_i or negedge rst_ni) begin
    if (!rst_ni) begin
      obi_rvalid_q <= 1'b0;
    end else begin
      obi_rvalid_q <= obi_gnt;
    end
  end
  assign obi_rsp_o = '{  // OBI Read Respons
          gnt   : obi_gnt,
          rvalid: obi_rvalid_q,
          rdata : {{32 - W{1'b0}}, obi_rdata_q}
      };


  //Regla #3 FIFO_0 --> AveraguePixel_Module
  assign fifo0_rd_en = ready_r_ave;
  assign valid_r_ave = ~fifo0_empty;
  assign datoin_ave = fifo0_dout;


  /*always_ff @(posedge clk_i or negedge rst_ni) begin
    if (ready_r_ave || valid_r_ave) begin
      if (ready_r_ave && valid_r_ave) begin
        $display("Dato_Va [%d] = %d", conta, datoin_ave);
      end else begin
        $display("Dato_In [%d]", conta);
      end
      conta <= conta + 1;
      if (conta == 2000) begin
        conta <= 0;
      end
    end
  end*/

  //Regla #4 AveraguePixel_Module --> FIFO_1
  assign fifo1_din = datout_ave;
  assign fifo1_wr_en = valid_w_ave;
  assign ready_w_ave = (!fifo1_full || fifo1_empty) ? 1'b1 : 1'b0;

  //Regla #5 FIFO_1 --> Canal Stream_Out_OBI
  localparam OBI_ADDR_DATAOUT = 32'hf000_0004;
  logic obi_gnt_2;
  logic obi_rvalid_q_2;
  logic [15:0] obi_rdata_q_2;
  //logic pending_read;

  //assign obi_gnt_2 = obi_req_i_2.req;
  assign obi_gnt_2 = obi_req_i_2.req && (obi_req_i_2.addr == OBI_ADDR_DATAOUT) && !fifo1_empty;
  //assign obi_gnt_2 = obi_req_i_2.req && !obi_req_i_2.we && (obi_req_i_2.addr == OBI_ADDR_DATAOUT) && !pending_read;
  //assign fifo1_rd_en = (pending_read && !fifo1_empty);
  assign fifo1_rd_en = obi_gnt_2 && !obi_req_i_2.we;
  assign obi_rdata_q_2 = fifo1_dout;

  always_ff @(posedge clk_i or negedge rst_ni) begin
    if (!rst_ni) begin
      //pending_read   <= 1'b0;
      obi_rvalid_q_2 <= 1'b0;
    end else begin
      //obi_rvalid_q_2 <= 1'b0;

      //if (obi_gnt_2) begin
      //  pending_read <= 1'b1;
      //$display("X1");
      //end

      //if (pending_read && !fifo1_empty) begin
      //  obi_rvalid_q_2 <= 1'b1;  // Avisamos al DMA que aquí está su dato
      //  pending_read   <= 1'b0;  // Ya no debemos nada
      //$display("X2");
      //end

      obi_rvalid_q_2 <= obi_gnt_2;
      //obi_rdata_q_2  <= fifo1_dout;
    end
  end

  assign obi_rsp_o_2 = '{  // OBI Read Respons
          gnt   : obi_gnt_2,
          rvalid: obi_rvalid_q_2,
          rdata : {{32 - W{1'b0}}, obi_rdata_q_2}
      };

endmodule
