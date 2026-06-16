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


module DupliTop (
    input rst,
    input clk,
    input start,
    output done,
    output ready_r,
    input valid_r,
    input [15:0] datoin,
    input ready_w,
    output valid_w,
    output [15:0] datout
);

  // Parámetros
  localparam DATA_SIZE = 16;  // log2(1024) = 10

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

  logic [DATA_SIZE-1:0] ImgBuff_out_din;
  logic ImgBuff_out_full_n;
  logic ImgBuff_out_write;
  logic [0:0] stage3;

  Avg_dup u_avg_dup (
      .ImgBuff_in_dout   (ImgBuff_in_dout),     //X
      .ImgBuff_in_empty_n(ImgBuff_in_empty_n),  //X
      .ImgBuff_in_read   (ImgBuff_in_read),     //X          
      .imgBuff_out_din   (ImgBuff_out_din),     //X 
      .imgBuff_out_full_n(ImgBuff_out_full_n),  //X
      .imgBuff_out_write (ImgBuff_out_write),   //X
      .stage3            (stage3),              //X
      .ap_clk            (clk),                 //X
      .ap_rst            (rst),                 //X
      .ap_start          (start),               //X
      .ap_done           (ap_done),             //X
      .ap_ready          (ap_ready),            //X
      .ap_idle           (ap_idle)              //X
  );


  assign datout = datout_r;
  assign datoin_r = datoin;
  assign done = ap_done;

  //senales de entrada
  assign ImgBuff_in_empty_n = valid_r;
  assign ready_r = ImgBuff_in_read;
  assign ImgBuff_in_dout = datoin_r;

  assign stage3 = 1'b0;

  //senales de salida
  assign valid_w = ImgBuff_out_write;
  assign ImgBuff_out_full_n = ready_w;
  assign datout_r = (ImgBuff_out_full_n == 1'b1 & ImgBuff_out_write) ? ImgBuff_out_din:
                      16'b0000000000000000;

endmodule

