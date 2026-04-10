module inoutt_obi #(
    parameter int unsigned W = 16  // ancho de datos
) (
    input logic clk_i,
    input logic rst_ni,

    input  inoutt_obi_pkg::obi_req_t  obi_req_i,
    output inoutt_obi_pkg::obi_resp_t obi_rsp_o,

    input  inoutt_obi_pkg::obi_req_t  obi_req_i_2,
    output inoutt_obi_pkg::obi_resp_t obi_rsp_o_2
);

  //debug
  logic [  6:0] conta1;  //puede contar hasta 2 a la 3
  logic [  6:0] conta2;  //puede contar hasta 2 a la 3


  // Señales internas
  logic [W-1:0] datoin;
  logic [W-1:0] datout;
  logic [W-1:0] datoin_reg;

  // Instancia del buffer
  inoutt u_inout (
      .clk   (clk_i),
      .rst   (~rst_ni),
      .datoin(datoin),
      .datout(datout)
  );

  // -------------------------------
  // OBI Write (capturar datoin)
  // -------------------------------
  localparam OBI_ADDR_DATAIN = 32'hf000_0000;

  logic obi_gnt;
  logic obi_rvalid_q = 0;
  logic [W-1:0] obi_rdata_q = 16'h0000;
  logic dummy_unused_be = |obi_req_i.be;



  assign obi_gnt = obi_req_i.req;

  always_ff @(posedge clk_i or negedge rst_ni) begin
    if (!rst_ni) begin
      datoin_reg <= '0;
      conta1 <= 7'h0;
      obi_rvalid_q <= 1'b0;
    end else begin
      obi_rvalid_q <= 1'b0;
      if (obi_req_i.req) begin
        if (obi_req_i.we) begin
          if (obi_req_i.addr == OBI_ADDR_DATAIN) begin
            $display(
                "T->[0x%d] OBI wdata= 0x%h, Obi req = 0x%h, OBI we = 0x%h, OBI addr = 0x%h, gnt = 0x%h, rvld = 0x%h, rdata = 0x%h",
                conta1, obi_req_i.wdata[W-1:0], obi_req_i.req, obi_req_i.we, obi_req_i.addr,
                obi_gnt, obi_rvalid_q, obi_rdata_q);
            datoin_reg   <= obi_req_i.wdata[W-1:0];
            obi_rvalid_q <= 1'b1;
          end
        end
      end
      conta1 <= conta1 + 1;
      if (conta1 == 100) begin
        conta1 <= 7'h0;
      end
    end
  end

  assign datoin = datoin_reg;


  // -------------------------------
  // OBI Respons
  // -------------------------------
  // Payload con la respuesta en cada solicitud OBI
  assign
      obi_rsp_o = '{gnt   : obi_gnt, rvalid: obi_rvalid_q, rdata : {{32 - W{1'b0}}, obi_rdata_q}};



  // -------------------------------
  // OBI Read (leer datout)
  // -------------------------------
  localparam OBI_ADDR_DATAOUT = 32'hf000_0004;

  logic obi_gnt_2;
  logic obi_rvalid_q_2;
  logic [W-1:0] obi_rdata_q_2;
  logic dummy_unused_be_2 = |obi_req_i_2.be;

  assign obi_gnt_2 = obi_req_i_2.req;

  always_ff @(posedge clk_i or negedge rst_ni) begin
    if (!rst_ni) begin
      conta2 <= 7'h0;
      obi_rvalid_q_2 <= 1'b0;
      obi_rdata_q_2 <= '0;
    end else begin
      obi_rvalid_q_2 <= 1'b0;
      if (obi_req_i_2.req) begin
        if (!obi_req_i_2.we) begin
          if (obi_req_i_2.addr == OBI_ADDR_DATAOUT) begin
            obi_rdata_q_2  <= datout;
            obi_rvalid_q_2 <= 1'b1;
            $display(
                "T <-[0x%d]: OBI rdata= 0x%h, Obi req = 0x%h, OBI we = 0x%h, OBI addr = 0x%h, gnt = 0x%h, rvld = 0x%h, rdata = 0x%h",
                conta2, obi_req_i_2.wdata[W-1:0], obi_req_i_2.req, obi_req_i_2.we,
                obi_req_i_2.addr, obi_gnt_2, obi_rvalid_q_2, obi_rdata_q_2);
          end
        end
      end
      conta2 <= conta2 + 1;
      if (conta2 == 100) begin
        conta2 <= 7'h0;
      end
    end
  end


  // -------------------------------
  // OBI Respons
  // -------------------------------
  // Payload con la respuesta en cada solicitud OBI
  assign obi_rsp_o_2 = '{
          gnt   : obi_gnt_2,
          rvalid: obi_rvalid_q_2,
          rdata : {{32 - W{1'b0}}, obi_rdata_q_2}
      };


endmodule



/*module inoutt_obi #(
    parameter int unsigned W = 16  // ancho de datos
) (
    input logic clk_i,
    input logic rst_ni,

    // Interfaz OBI (acceso al valor de salida)
    input  inoutt_obi_pkg::obi_req_t  obi_req_i,
    output inoutt_obi_pkg::obi_resp_t obi_rsp_o

);

  // Señales internas
  logic [W-1:0] datoin;
  logic [W-1:0] datout;
  logic [W-1:0] datoin_reg;

  // Instancia del buffer
  inoutt u_inout (
      .clk   (clk_i),
      .rst   (~rst_ni),
      .datoin(datoin),
      .datout(datout)
  );

  // -------------------------------
  // OBI Write (capturar datoin)
  // -------------------------------
  localparam OBI_ADDR_DATAIN = 32'hf000_0000;

  always_ff @(posedge clk_i or negedge rst_ni) begin
    if (!rst_ni) begin
      datoin_reg <= '0;
    end else begin
      if (obi_req_i.req) begin
        if (obi_req_i.we) begin
          $display("Request = 0x%h, WR = 0x%h, address = 0x%h", obi_req_i.req, obi_req_i.we,
                   obi_req_i.addr);
          if (obi_req_i.addr == OBI_ADDR_DATAIN) begin
            $display(
                "OBI Dato write in module= 0x%h, Obi req = 0x%h, OBI we = 0x%h, OBI addr = 0x%h,",
                obi_req_i.wdata[W-1:0], obi_req_i.req, obi_req_i.we, obi_req_i.addr);
            datoin_reg <= obi_req_i.wdata[W-1:0];
          end
        end
      end
    end
  end

  logic dummy_unused_be = |obi_req_i.be;
  assign datoin = datoin_reg;

  // -------------------------------
  // OBI Read (leer datout)
  // -------------------------------
  localparam OBI_ADDR_DATAOUT = 32'hf000_0004;

  logic obi_gnt;
  logic obi_rvalid_q;
  logic [W-1:0] obi_rdata_q;


  assign obi_gnt = obi_req_i.req;

  always_ff @(posedge clk_i or negedge rst_ni) begin
    if (obi_req_i.req) begin
      if (!obi_req_i.we) begin
        if (obi_req_i.addr == OBI_ADDR_DATAOUT) begin
          obi_rdata_q <= datout;
          $display("OBI Dato read of module= 0x%h, Obi req = 0x%h, OBI we = 0x%h, OBI addr = 0x%h,",
                   obi_req_i.wdata[W-1:0], obi_req_i.req, obi_req_i.we, obi_req_i.addr);
        end
      end
    end
    if (!rst_ni) begin
      obi_rvalid_q <= 1'b0;
      obi_rdata_q  <= '0;
    end else begin
      obi_rvalid_q <= obi_gnt;
    end
  end


  // -------------------------------
  // OBI Respons
  // -------------------------------
  // Payload con la respuesta en cada solicitud OBI
  assign
      obi_rsp_o = '{gnt   : obi_gnt, rvalid: obi_rvalid_q, rdata : {{32 - W{1'b0}}, obi_rdata_q}};


endmodule
*/
