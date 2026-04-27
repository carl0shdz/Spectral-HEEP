module FIFO_V1 (
    input logic clk,
    input logic rst_n,

    input logic        wr_en,
    input logic        rd_en,
    input logic [15:0] din,

    output logic [15:0] dout,
    output logic        full,
    output logic        empty
);

  // Parámetros
  localparam DATA_WIDTH = 16;
  //localparam DEPTH = 1920;
  //localparam DEPTH = 512;
  localparam DEPTH = 128;
  //localparam DEPTH = 32;
  //localparam DEPTH = 4;
  //localparam DEPTH = 1;

  localparam ADDR_WIDTH = 7;  // log2(DEPTH*BANDS) = 18

  // Memoria FIFO
  logic [DATA_WIDTH-1:0] mem[0:DEPTH-1];

  // Punteros
  logic [ADDR_WIDTH-1:0] wr_ptr;
  logic [ADDR_WIDTH-1:0] rd_ptr;

  // Contador de elementos
  logic [ADDR_WIDTH:0] count;

  // Escritura
  always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
      wr_ptr <= '0;
    end else if (wr_en && !full) begin
      mem[wr_ptr] <= din;
      if (wr_ptr == 1 || wr_ptr == 2) begin
        $display("F1 IN = %d, Index = %d", din, wr_ptr);
      end
      if (wr_ptr == DEPTH - 1) begin
        wr_ptr <= '0;
      end else begin
        wr_ptr <= wr_ptr + 1'b1;
      end
    end
  end


  always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
      rd_ptr <= '0;
    end else if (rd_en && !empty) begin
      if (rd_ptr == 1 || rd_ptr == 2) begin
        $display("F1 OUT = %d, Index = %d", mem[rd_ptr], rd_ptr);
      end
      if (rd_ptr == DEPTH - 1) begin
        rd_ptr <= '0;
      end else begin
        rd_ptr <= rd_ptr + 1'b1;
      end
    end
  end

  assign dout = empty ? 16'd0 : mem[rd_ptr];


  always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
      count <= '0;
    end else begin
      case ({
        wr_en && !full, rd_en && !empty
      })
        2'b10:   count <= count + 1'b1;  // solo escritura
        2'b01:   count <= count - 1'b1;  // solo lectura
        default: count <= count;  // nada o ambas
      endcase
    end
  end

  // Flags
  assign full  = (count == DEPTH);
  assign empty = (count == 0);

endmodule

