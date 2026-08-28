module FIFO_V2 (
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
  localparam DEPTH = 16;  //120*16

  localparam ADDR_WIDTH = 4;  // log2(DEPTH*BANDS) = 18

  // Memoria FIFO
  logic [DATA_WIDTH-1:0] mem[0:DEPTH-1];

  // Punteros
  logic [ADDR_WIDTH-1:0] wr_ptr;
  logic [ADDR_WIDTH-1:0] rd_ptr;

  // Contador de elementos
  logic [ADDR_WIDTH:0] count;  // 11 bits para contar hasta 1024

  // Escritura
  always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
      wr_ptr <= '0;
    end else if (wr_en && !full) begin
      mem[wr_ptr] <= din;
      $display("F2 IN = %d, Index = %d", din, wr_ptr);
      wr_ptr <= wr_ptr + 1'b1;
    end
  end

  // Lectura
  always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
      rd_ptr <= '0;
      dout   <= '0;
    end else if (rd_en && !empty) begin
      dout <= mem[rd_ptr];
      $display("F2 OUT = %d, Index = %d", mem[rd_ptr], rd_ptr);
      rd_ptr <= rd_ptr + 1'b1;
    end
  end

  // Contador de elementos
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
