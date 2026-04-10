module inoutt (
    input logic clk,
    input logic rst,
    input logic [15:0] datoin,
    output logic [15:0] datout
);

  logic [15:0] cable1;

  always_ff @(posedge clk or posedge rst) begin
    if (rst) begin
      cable1 <= 'h0;
    end else begin
      cable1 <= datoin + 1;
    end
  end

  assign datout = cable1;

endmodule
