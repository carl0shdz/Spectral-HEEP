//`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 01/13/2026 11:23:06 AM
// Design Name: 
// Module Name: AveragePixelTop
// Project Name: 
// Target Devices: Nexys a7 100t
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module SubtractionPixelTop (
    input rst,
    input clk,
    input start,
    output done,
    output ready_r,
    input valid_r,
    input [31:0] datoin,
    output ready_r_pr,
    input valid_r_pr,
    input [31:0] datoin_pr,
    input ready_w,
    output valid_w,
    output [31:0] datout
);

  // Parámetros
  localparam BANDS = 16;
  localparam BLOCK_SIZE = 120;
  localparam DATA_SIZE = 32;  // log2(1024) = 10

  //Senales modulo
  logic [DATA_SIZE-1:0] datoin_r;
  logic [DATA_SIZE-1:0] datoin_r_pr;
  logic [DATA_SIZE-1:0] datout_r;

  // Senales internas
  logic ap_done;
  logic ap_idle;
  logic ap_ready;
  logic [DATA_SIZE-1:0] in_Buffer_img_dout;
  logic in_Buffer_img_empty_n;
  logic in_Buffer_img_read;
  logic [DATA_SIZE-1:0] projection_dout;
  logic projection_empty_n;
  logic projection_read;

  logic [DATA_SIZE-1:0] out_Buffer_img_din;
  logic out_Buffer_img_full_n;
  logic out_Buffer_img_write;
  logic [0:0] stage3;

  //instancia del modulo verilog
  SUBTRACTION u_SUBTRACTION (
      .ap_clk(clk),
      .ap_rst(rst),
      .ap_start(start),
      .ap_done(ap_done),
      .ap_idle(ap_idle),
      .ap_ready(ap_ready),
      .in_Buffer_img_dout(in_Buffer_img_dout),
      .in_Buffer_img_empty_n(in_Buffer_img_empty_n),
      .in_Buffer_img_read(in_Buffer_img_read),
      .projection_dout(projection_dout),
      .projection_empty_n(projection_empty_n),
      .projection_read(projection_read),
      .out_Buffer_img_din(out_Buffer_img_din),
      .out_Buffer_img_full_n(out_Buffer_img_full_n),
      .out_Buffer_img_write(out_Buffer_img_write),
      .stage3(stage3)
  );
  // =========================
  // Salida de estado (opcional)
  // =========================


  //assign FSM = current_state;
  assign datout = datout_r;
  assign datoin_r = datoin;
  assign datoin_r_pr = datoin_pr;
  assign done = ap_done;

  //senales de entrada
  assign in_Buffer_img_empty_n = valid_r;
  assign ready_r = in_Buffer_img_read;
  assign stage3 = 1'b0;
  assign in_Buffer_img_dout = datoin_r;

  assign projection_empty_n = valid_r_pr;
  assign ready_r_pr = projection_read;
  assign projection_dout = datoin_r_pr;

  //senales de salida
  assign valid_w = out_Buffer_img_write;
  assign out_Buffer_img_full_n = ready_w;
  assign datout_r = (out_Buffer_img_full_n == 1'b1 & out_Buffer_img_write) ? out_Buffer_img_din:
                      32'b00000000000000000000000000000000;

  //testing

  /*always_ff @(posedge clk or negedge rst) begin : Testing
    if (rst) begin
      $display("rst");
    end else begin
      if (start) begin
        //if (in_Buffer_img_read && in_Buffer_img_empty_n) begin
        //    $display("Data: %d", in_Buffer_img_dout);
        //end
        //if (projection_read && projection_empty_n) begin
        //    $display("Projection: %d", projection_dout);
        //end
        if (out_Buffer_img_write && out_Buffer_img_full_n) begin
          $display("Output: %d", out_Buffer_img_din);
        end
        //if (ap_done) begin
        //    $display("Done");
        //end
      end
    end
  end*/
endmodule
