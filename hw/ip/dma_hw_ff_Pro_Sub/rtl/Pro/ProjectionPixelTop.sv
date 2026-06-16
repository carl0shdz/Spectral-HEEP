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


module ProjectionPixelTop (
    input rst,
    input clk,
    input start,
    output done,
    output ready_r,
    input valid_r,
    input [31:0] datoin,
    input ready_w_pro,
    output valid_w_pro,
    output [31:0] datout_pro,
    input ready_w_x,
    output valid_w_x,
    output [31:0] datout_x,
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
  logic [DATA_SIZE-1:0] datout_r_pro;
  logic [DATA_SIZE-1:0] datout_r_x;

  // Senales internas
  logic ap_done;
  logic ap_idle;
  logic ap_ready;
  logic [DATA_SIZE-1:0] in_Buffer_img_dout;
  logic in_Buffer_img_empty_n;
  logic in_Buffer_img_read;

  logic [DATA_SIZE-1:0] out_Buffer_img_din;
  logic out_Buffer_img_full_n;
  logic out_Buffer_img_write;

  logic [DATA_SIZE-1:0] Buffer_Proj_din;
  logic Buffer_Proj_full_n;
  logic Buffer_Proj_write;

  logic [DATA_SIZE-1:0] out_projection_din;
  logic out_projection_full_n;
  logic out_projection_write;

  logic [DATA_SIZE-1:0] DRPROXECTIONS;

  logic [0:0] stage3;

  //instancia del modulo verilog
  PROJECTION u_PROJECTION (
      .in_Buffer_img_dout (in_Buffer_img_dout),
      .in_Buffer_img_empty_n (in_Buffer_img_empty_n),
      .in_Buffer_img_read (in_Buffer_img_read),
      .out_Buffer_img_din (out_Buffer_img_din),
      .out_Buffer_img_full_n (out_Buffer_img_full_n),
      .out_Buffer_img_write (out_Buffer_img_write),
      .Buffer_Proj_din  (Buffer_Proj_din),
      .Buffer_Proj_full_n (Buffer_Proj_full_n),
      .Buffer_Proj_write (Buffer_Proj_write),
      .out_projection_din (out_projection_din),
      .out_projection_full_n (out_projection_full_n),
      .out_projection_write (out_projection_write),
      .DRPROXECTIONS  (DRPROXECTIONS),
      .stage3   (stage3),
      .ap_clk   (clk),
      .ap_rst   (rst),
      .ap_start  (start),
      .ap_done   (ap_done),
      .ap_ready  (ap_ready),
      .ap_idle   (ap_idle)
  );
  // =========================
  // Salida de estado (opcional)
  // =========================


  //assign FSM = current_state;
  assign datout = datout_r;
  assign datout_pro = datout_r_pro;
  assign datout_x = datout_r_x;
  assign datoin_r = datoin;
  assign done = ap_done;

  //senales de entrada
  assign in_Buffer_img_empty_n = valid_r;
  assign ready_r = in_Buffer_img_read;
  assign stage3 = 1'b0;
  assign in_Buffer_img_dout = datoin_r;
  assign DRPROXECTIONS = 32'b00000000000000000000000000000010;


  //senales de salida
  assign valid_w_pro = Buffer_Proj_write;
  assign Buffer_Proj_full_n = ready_w_pro;
  assign datout_r_pro = (Buffer_Proj_full_n == 1'b1 & Buffer_Proj_write) ? Buffer_Proj_din:
                      32'b00000000000000000000000000000000;

  //Bus vectores Q, U, bloque R
  assign valid_w = out_Buffer_img_write;
  assign out_Buffer_img_full_n = ready_w;
  assign datout_r = (out_Buffer_img_full_n == 1'b1 & out_Buffer_img_write) ? out_Buffer_img_din:
                      32'b00000000000000000000000000000000;

  //senales de salida
  assign valid_w_x = out_projection_write;
  assign out_projection_full_n = ready_w_x;
  assign datout_r_x = (out_projection_full_n == 1'b1 & out_projection_write) ? out_projection_din:
                      32'b00000000000000000000000000000000;

endmodule
