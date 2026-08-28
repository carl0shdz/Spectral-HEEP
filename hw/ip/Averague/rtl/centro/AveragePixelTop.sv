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


module AveragePixelTop (
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
  localparam BANDS = 16;
  localparam BLOCK_SIZE = 120;
  localparam DATA_SIZE = 16;  // log2(1024) = 10

  //Senales modulo
  logic [DATA_SIZE-1:0] datoin_r;
  logic [DATA_SIZE-1:0] datout_r;

  // Senales internas
  logic ap_done;
  logic ap_idle;
  logic ap_ready;
  logic [DATA_SIZE-1:0] ImgRef_dout;
  logic ImgRef_empty_n;
  logic ImgRef_read;
  logic [DATA_SIZE-1:0] out_centroid_din;
  logic out_centroid_full_n;
  logic out_centroid_write;
  logic [0:0] stage3;

  //instancia del modulo verilog
  Avg_Cent u_avg_cent (
      .ap_clk(clk),
      .ap_rst(rst),
      .ap_start(start),
      .ap_done(ap_done),
      .ap_idle(ap_idle),
      .ap_ready(ap_ready),
      .ImgRef_dout(ImgRef_dout),
      .ImgRef_empty_n(ImgRef_empty_n),
      .ImgRef_read(ImgRef_read),
      .out_centroid_din(out_centroid_din),
      .out_centroid_full_n(out_centroid_full_n),
      .out_centroid_write(out_centroid_write),
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
  assign ImgRef_empty_n = valid_r;
  assign ready_r = ImgRef_read;
  assign stage3 = 1'b0;
  assign ImgRef_dout = datoin_r;

  //senales de salida
  assign valid_w = out_centroid_write;
  assign out_centroid_full_n = ready_w;
  assign datout_r = (out_centroid_full_n == 1'b1 & out_centroid_write) ? out_centroid_din:
                      16'b0000000000000000;

endmodule

