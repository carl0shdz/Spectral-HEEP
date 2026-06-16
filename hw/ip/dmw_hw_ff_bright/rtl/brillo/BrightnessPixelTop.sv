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


module BrightnessPixelTop (
    input rst,
    input clk,
    input start,
    output done,
    output ready_r,
    input valid_r,
    input [31:0] datoin,
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
  logic [DATA_SIZE-1:0] datout_r;

  // Senales internas
  logic ap_done;
  logic ap_idle;
  logic ap_ready;
  logic [DATA_SIZE-1:0] ImgBuff_in_dout;
  logic ImgBuff_in_empty_n;
  logic ImgBuff_in_read;
  logic [DATA_SIZE-1:0] Vector_b_din;
  logic Vector_b_full_n;
  logic Vector_b_write;
  logic [0:0] stage3;

  //instancia del modulo verilog
  BRIGHT u_BRIGHT (
      .ap_clk(clk),
      .ap_rst(rst),
      .ap_start(start),
      .ap_done(ap_done),
      .ap_idle(ap_idle),
      .ap_ready(ap_ready),
      .ImgBuff_in_dout(ImgBuff_in_dout),
      .ImgBuff_in_empty_n(ImgBuff_in_empty_n),
      .ImgBuff_in_read(ImgBuff_in_read),
      .Vector_b_din(Vector_b_din),
      .Vector_b_full_n(Vector_b_full_n),
      .Vector_b_write(Vector_b_write),
      .stage3(stage3)
  );
  // =========================
  // Salida de estado (opcional)
  // =========================


  //assign FSM = current_state;
  assign datout = datout_r;
  assign datoin_r = datoin;
  assign done = ap_done;

  //senales de entrada
  assign ImgBuff_in_empty_n = valid_r;
  assign ready_r = ImgBuff_in_read;
  assign stage3 = 1'b0;
  assign ImgBuff_in_dout = datoin_r;

  //senales de salida
  assign valid_w = Vector_b_write;
  assign Vector_b_full_n = ready_w;
  assign datout_r = (Vector_b_full_n == 1'b1 & Vector_b_write) ? Vector_b_din:
                      32'b00000000000000000000000000000000;

endmodule
