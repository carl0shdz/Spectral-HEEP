

package averague_reg_pkg;

  typedef struct packed {
    logic        valid;
    logic        write;
    logic [3:0]  wstrb;
    logic [31:0] addr;
    logic [31:0] wdata;
  } reg_req_t;

  typedef struct packed {
    logic        error;
    logic        ready;
    logic [31:0] rdata;
  } reg_resp_t;

endpackage
