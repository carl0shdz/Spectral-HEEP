

package averague_obi_pkg;

  typedef struct packed {
    logic        req;
    logic        we;
    logic [3:0]  be;
    logic [31:0] addr;
    logic [31:0] wdata;
  } obi_req_t;

  typedef struct packed {
    logic        gnt;
    logic        rvalid;
    logic [31:0] rdata;
  } obi_resp_t;

endpackage
