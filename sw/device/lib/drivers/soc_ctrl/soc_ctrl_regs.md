<table class="regdef" id="Reg_exit_valid">
 <tr>
  <th class="regdef" colspan=5>
   <div>soc_ctrl.EXIT_VALID @ 0x0</div>
   <div><p>Exit Valid - Used to write exit valid bit</p></div>
   <div>Reset default = 0x0, mask 0x1</div>
  </th>
 </tr>
<tr><td colspan=5><table class="regpic"><tr><td class="bitnum">31</td><td class="bitnum">30</td><td class="bitnum">29</td><td class="bitnum">28</td><td class="bitnum">27</td><td class="bitnum">26</td><td class="bitnum">25</td><td class="bitnum">24</td><td class="bitnum">23</td><td class="bitnum">22</td><td class="bitnum">21</td><td class="bitnum">20</td><td class="bitnum">19</td><td class="bitnum">18</td><td class="bitnum">17</td><td class="bitnum">16</td></tr><tr><td class="unused" colspan=16>&nbsp;</td>
</tr>
<tr><td class="bitnum">15</td><td class="bitnum">14</td><td class="bitnum">13</td><td class="bitnum">12</td><td class="bitnum">11</td><td class="bitnum">10</td><td class="bitnum">9</td><td class="bitnum">8</td><td class="bitnum">7</td><td class="bitnum">6</td><td class="bitnum">5</td><td class="bitnum">4</td><td class="bitnum">3</td><td class="bitnum">2</td><td class="bitnum">1</td><td class="bitnum">0</td></tr><tr><td class="unused" colspan=15>&nbsp;</td>
<td class="fname" colspan=1 style="font-size:30.0%">EXIT_VALID</td>
</tr></table></td></tr>
<tr><th width=5%>Bits</th><th width=5%>Type</th><th width=5%>Reset</th><th>Name</th><th>Description</th></tr><tr><td class="regbits">0</td><td class="regperm">rw</td><td class="regrv">x</td><td class="regfn">EXIT_VALID</td><td class="regde"><p>Exit Valid Reg</p></td></table>
<br>
<table class="regdef" id="Reg_exit_value">
 <tr>
  <th class="regdef" colspan=5>
   <div>soc_ctrl.EXIT_VALUE @ 0x4</div>
   <div><p>Exit Value - Used to write exit value register</p></div>
   <div>Reset default = 0x0, mask 0xffffffff</div>
  </th>
 </tr>
<tr><td colspan=5><table class="regpic"><tr><td class="bitnum">31</td><td class="bitnum">30</td><td class="bitnum">29</td><td class="bitnum">28</td><td class="bitnum">27</td><td class="bitnum">26</td><td class="bitnum">25</td><td class="bitnum">24</td><td class="bitnum">23</td><td class="bitnum">22</td><td class="bitnum">21</td><td class="bitnum">20</td><td class="bitnum">19</td><td class="bitnum">18</td><td class="bitnum">17</td><td class="bitnum">16</td></tr><tr><td class="fname" colspan=16>EXIT_VALUE...</td>
</tr>
<tr><td class="bitnum">15</td><td class="bitnum">14</td><td class="bitnum">13</td><td class="bitnum">12</td><td class="bitnum">11</td><td class="bitnum">10</td><td class="bitnum">9</td><td class="bitnum">8</td><td class="bitnum">7</td><td class="bitnum">6</td><td class="bitnum">5</td><td class="bitnum">4</td><td class="bitnum">3</td><td class="bitnum">2</td><td class="bitnum">1</td><td class="bitnum">0</td></tr><tr><td class="fname" colspan=16>...EXIT_VALUE</td>
</tr></table></td></tr>
<tr><th width=5%>Bits</th><th width=5%>Type</th><th width=5%>Reset</th><th>Name</th><th>Description</th></tr><tr><td class="regbits">31:0</td><td class="regperm">rw</td><td class="regrv">x</td><td class="regfn">EXIT_VALUE</td><td class="regde"><p>Exit Value Reg</p></td></table>
<br>
<table class="regdef" id="Reg_boot_select">
 <tr>
  <th class="regdef" colspan=5>
   <div>soc_ctrl.BOOT_SELECT @ 0x8</div>
   <div><p>Boot Select Value - Used to decide whether to boot from JTAG or FLASH</p></div>
   <div>Reset default = 0x0, mask 0x1</div>
  </th>
 </tr>
<tr><td colspan=5><table class="regpic"><tr><td class="bitnum">31</td><td class="bitnum">30</td><td class="bitnum">29</td><td class="bitnum">28</td><td class="bitnum">27</td><td class="bitnum">26</td><td class="bitnum">25</td><td class="bitnum">24</td><td class="bitnum">23</td><td class="bitnum">22</td><td class="bitnum">21</td><td class="bitnum">20</td><td class="bitnum">19</td><td class="bitnum">18</td><td class="bitnum">17</td><td class="bitnum">16</td></tr><tr><td class="unused" colspan=16>&nbsp;</td>
</tr>
<tr><td class="bitnum">15</td><td class="bitnum">14</td><td class="bitnum">13</td><td class="bitnum">12</td><td class="bitnum">11</td><td class="bitnum">10</td><td class="bitnum">9</td><td class="bitnum">8</td><td class="bitnum">7</td><td class="bitnum">6</td><td class="bitnum">5</td><td class="bitnum">4</td><td class="bitnum">3</td><td class="bitnum">2</td><td class="bitnum">1</td><td class="bitnum">0</td></tr><tr><td class="unused" colspan=15>&nbsp;</td>
<td class="fname" colspan=1 style="font-size:27.272727272727273%">BOOT_SELECT</td>
</tr></table></td></tr>
<tr><th width=5%>Bits</th><th width=5%>Type</th><th width=5%>Reset</th><th>Name</th><th>Description</th></tr><tr><td class="regbits">0</td><td class="regperm">ro</td><td class="regrv">x</td><td class="regfn">BOOT_SELECT</td><td class="regde"><p>Boot Select Reg</p></td></table>
<br>
<table class="regdef" id="Reg_boot_exit_loop">
 <tr>
  <th class="regdef" colspan=5>
   <div>soc_ctrl.BOOT_EXIT_LOOP @ 0xc</div>
   <div><p>Boot Exit Loop Value - Set externally (e.g. JTAG, TESTBENCH, or another MASTER) to make the CPU jump to the main function entry</p></div>
   <div>Reset default = 0x0, mask 0x1</div>
  </th>
 </tr>
<tr><td colspan=5><table class="regpic"><tr><td class="bitnum">31</td><td class="bitnum">30</td><td class="bitnum">29</td><td class="bitnum">28</td><td class="bitnum">27</td><td class="bitnum">26</td><td class="bitnum">25</td><td class="bitnum">24</td><td class="bitnum">23</td><td class="bitnum">22</td><td class="bitnum">21</td><td class="bitnum">20</td><td class="bitnum">19</td><td class="bitnum">18</td><td class="bitnum">17</td><td class="bitnum">16</td></tr><tr><td class="unused" colspan=16>&nbsp;</td>
</tr>
<tr><td class="bitnum">15</td><td class="bitnum">14</td><td class="bitnum">13</td><td class="bitnum">12</td><td class="bitnum">11</td><td class="bitnum">10</td><td class="bitnum">9</td><td class="bitnum">8</td><td class="bitnum">7</td><td class="bitnum">6</td><td class="bitnum">5</td><td class="bitnum">4</td><td class="bitnum">3</td><td class="bitnum">2</td><td class="bitnum">1</td><td class="bitnum">0</td></tr><tr><td class="unused" colspan=15>&nbsp;</td>
<td class="fname" colspan=1 style="font-size:21.428571428571427%">BOOT_EXIT_LOOP</td>
</tr></table></td></tr>
<tr><th width=5%>Bits</th><th width=5%>Type</th><th width=5%>Reset</th><th>Name</th><th>Description</th></tr><tr><td class="regbits">0</td><td class="regperm">rw</td><td class="regrv">x</td><td class="regfn">BOOT_EXIT_LOOP</td><td class="regde"><p>Boot Exit Reg</p></td></table>
<br>
<table class="regdef" id="Reg_boot_address">
 <tr>
  <th class="regdef" colspan=5>
   <div>soc_ctrl.BOOT_ADDRESS @ 0x10</div>
   <div><p>Boot Address Value - Used in the boot rom or power-on-reset functions</p></div>
   <div>Reset default = 0x180, mask 0xffffffff</div>
  </th>
 </tr>
<tr><td colspan=5><table class="regpic"><tr><td class="bitnum">31</td><td class="bitnum">30</td><td class="bitnum">29</td><td class="bitnum">28</td><td class="bitnum">27</td><td class="bitnum">26</td><td class="bitnum">25</td><td class="bitnum">24</td><td class="bitnum">23</td><td class="bitnum">22</td><td class="bitnum">21</td><td class="bitnum">20</td><td class="bitnum">19</td><td class="bitnum">18</td><td class="bitnum">17</td><td class="bitnum">16</td></tr><tr><td class="fname" colspan=16>BOOT_ADDRESS...</td>
</tr>
<tr><td class="bitnum">15</td><td class="bitnum">14</td><td class="bitnum">13</td><td class="bitnum">12</td><td class="bitnum">11</td><td class="bitnum">10</td><td class="bitnum">9</td><td class="bitnum">8</td><td class="bitnum">7</td><td class="bitnum">6</td><td class="bitnum">5</td><td class="bitnum">4</td><td class="bitnum">3</td><td class="bitnum">2</td><td class="bitnum">1</td><td class="bitnum">0</td></tr><tr><td class="fname" colspan=16>...BOOT_ADDRESS</td>
</tr></table></td></tr>
<tr><th width=5%>Bits</th><th width=5%>Type</th><th width=5%>Reset</th><th>Name</th><th>Description</th></tr><tr><td class="regbits">31:0</td><td class="regperm">rw</td><td class="regrv">0x180</td><td class="regfn">BOOT_ADDRESS</td><td class="regde"><p>Boot Address Reg</p></td></table>
<br>
<table class="regdef" id="Reg_use_spimemio">
 <tr>
  <th class="regdef" colspan=5>
   <div>soc_ctrl.USE_SPIMEMIO @ 0x14</div>
   <div><p>Spi Module Select Value - Used to decide whether to use the SPI from Yosys or OpenTitan</p></div>
   <div>Reset default = 0x1, mask 0x1</div>
  </th>
 </tr>
<tr><td colspan=5><table class="regpic"><tr><td class="bitnum">31</td><td class="bitnum">30</td><td class="bitnum">29</td><td class="bitnum">28</td><td class="bitnum">27</td><td class="bitnum">26</td><td class="bitnum">25</td><td class="bitnum">24</td><td class="bitnum">23</td><td class="bitnum">22</td><td class="bitnum">21</td><td class="bitnum">20</td><td class="bitnum">19</td><td class="bitnum">18</td><td class="bitnum">17</td><td class="bitnum">16</td></tr><tr><td class="unused" colspan=16>&nbsp;</td>
</tr>
<tr><td class="bitnum">15</td><td class="bitnum">14</td><td class="bitnum">13</td><td class="bitnum">12</td><td class="bitnum">11</td><td class="bitnum">10</td><td class="bitnum">9</td><td class="bitnum">8</td><td class="bitnum">7</td><td class="bitnum">6</td><td class="bitnum">5</td><td class="bitnum">4</td><td class="bitnum">3</td><td class="bitnum">2</td><td class="bitnum">1</td><td class="bitnum">0</td></tr><tr><td class="unused" colspan=15>&nbsp;</td>
<td class="fname" colspan=1 style="font-size:25.0%">USE_SPIMEMIO</td>
</tr></table></td></tr>
<tr><th width=5%>Bits</th><th width=5%>Type</th><th width=5%>Reset</th><th>Name</th><th>Description</th></tr><tr><td class="regbits">0</td><td class="regperm">rw</td><td class="regrv">0x1</td><td class="regfn">USE_SPIMEMIO</td><td class="regde"><p>Spi Select Reg</p></td></table>
<br>
<table class="regdef" id="Reg_enable_spi_sel">
 <tr>
  <th class="regdef" colspan=5>
   <div>soc_ctrl.ENABLE_SPI_SEL @ 0x18</div>
   <div><p>Enable Spi module selection from software</p></div>
   <div>Reset default = 0x0, mask 0x1</div>
  </th>
 </tr>
<tr><td colspan=5><table class="regpic"><tr><td class="bitnum">31</td><td class="bitnum">30</td><td class="bitnum">29</td><td class="bitnum">28</td><td class="bitnum">27</td><td class="bitnum">26</td><td class="bitnum">25</td><td class="bitnum">24</td><td class="bitnum">23</td><td class="bitnum">22</td><td class="bitnum">21</td><td class="bitnum">20</td><td class="bitnum">19</td><td class="bitnum">18</td><td class="bitnum">17</td><td class="bitnum">16</td></tr><tr><td class="unused" colspan=16>&nbsp;</td>
</tr>
<tr><td class="bitnum">15</td><td class="bitnum">14</td><td class="bitnum">13</td><td class="bitnum">12</td><td class="bitnum">11</td><td class="bitnum">10</td><td class="bitnum">9</td><td class="bitnum">8</td><td class="bitnum">7</td><td class="bitnum">6</td><td class="bitnum">5</td><td class="bitnum">4</td><td class="bitnum">3</td><td class="bitnum">2</td><td class="bitnum">1</td><td class="bitnum">0</td></tr><tr><td class="unused" colspan=15>&nbsp;</td>
<td class="fname" colspan=1 style="font-size:21.428571428571427%">ENABLE_SPI_SEL</td>
</tr></table></td></tr>
<tr><th width=5%>Bits</th><th width=5%>Type</th><th width=5%>Reset</th><th>Name</th><th>Description</th></tr><tr><td class="regbits">0</td><td class="regperm">rw</td><td class="regrv">0x0</td><td class="regfn">ENABLE_SPI_SEL</td><td class="regde"><p>Enable Spi Select Reg</p></td></table>
<br>
<table class="regdef" id="Reg_system_frequency_hz">
 <tr>
  <th class="regdef" colspan=5>
   <div>soc_ctrl.SYSTEM_FREQUENCY_HZ @ 0x1c</div>
   <div><p>System Frequency Value - Used to know and set at which frequency the system is running (in Hz)</p></div>
   <div>Reset default = 0x1, mask 0xffffffff</div>
  </th>
 </tr>
<tr><td colspan=5><table class="regpic"><tr><td class="bitnum">31</td><td class="bitnum">30</td><td class="bitnum">29</td><td class="bitnum">28</td><td class="bitnum">27</td><td class="bitnum">26</td><td class="bitnum">25</td><td class="bitnum">24</td><td class="bitnum">23</td><td class="bitnum">22</td><td class="bitnum">21</td><td class="bitnum">20</td><td class="bitnum">19</td><td class="bitnum">18</td><td class="bitnum">17</td><td class="bitnum">16</td></tr><tr><td class="fname" colspan=16>SYSTEM_FREQUENCY_HZ...</td>
</tr>
<tr><td class="bitnum">15</td><td class="bitnum">14</td><td class="bitnum">13</td><td class="bitnum">12</td><td class="bitnum">11</td><td class="bitnum">10</td><td class="bitnum">9</td><td class="bitnum">8</td><td class="bitnum">7</td><td class="bitnum">6</td><td class="bitnum">5</td><td class="bitnum">4</td><td class="bitnum">3</td><td class="bitnum">2</td><td class="bitnum">1</td><td class="bitnum">0</td></tr><tr><td class="fname" colspan=16>...SYSTEM_FREQUENCY_HZ</td>
</tr></table></td></tr>
<tr><th width=5%>Bits</th><th width=5%>Type</th><th width=5%>Reset</th><th>Name</th><th>Description</th></tr><tr><td class="regbits">31:0</td><td class="regperm">rw</td><td class="regrv">0x1</td><td class="regfn">SYSTEM_FREQUENCY_HZ</td><td class="regde"><p>Contains the value in Hz of the frequency the system is running</p></td></table>
<br>
<table class="regdef" id="Reg_xheep_id">
 <tr>
  <th class="regdef" colspan=5>
   <div>soc_ctrl.XHEEP_ID @ 0x20</div>
   <div><p>In case of multiple X-HEEP instances, it tells you the X-HEEP instance id</p></div>
   <div>Reset default = 0x0, mask 0xffffffff</div>
  </th>
 </tr>
<tr><td colspan=5><table class="regpic"><tr><td class="bitnum">31</td><td class="bitnum">30</td><td class="bitnum">29</td><td class="bitnum">28</td><td class="bitnum">27</td><td class="bitnum">26</td><td class="bitnum">25</td><td class="bitnum">24</td><td class="bitnum">23</td><td class="bitnum">22</td><td class="bitnum">21</td><td class="bitnum">20</td><td class="bitnum">19</td><td class="bitnum">18</td><td class="bitnum">17</td><td class="bitnum">16</td></tr><tr><td class="fname" colspan=16>XHEEP_ID...</td>
</tr>
<tr><td class="bitnum">15</td><td class="bitnum">14</td><td class="bitnum">13</td><td class="bitnum">12</td><td class="bitnum">11</td><td class="bitnum">10</td><td class="bitnum">9</td><td class="bitnum">8</td><td class="bitnum">7</td><td class="bitnum">6</td><td class="bitnum">5</td><td class="bitnum">4</td><td class="bitnum">3</td><td class="bitnum">2</td><td class="bitnum">1</td><td class="bitnum">0</td></tr><tr><td class="fname" colspan=16>...XHEEP_ID</td>
</tr></table></td></tr>
<tr><th width=5%>Bits</th><th width=5%>Type</th><th width=5%>Reset</th><th>Name</th><th>Description</th></tr><tr><td class="regbits">31:0</td><td class="regperm">ro</td><td class="regrv">x</td><td class="regfn">XHEEP_ID</td><td class="regde"><p>X-HEEP instance id</p></td></table>
<br>
<table class="regdef" id="Reg_xheep_ao_peripheral_config">
 <tr>
  <th class="regdef" colspan=5>
   <div>soc_ctrl.XHEEP_AO_PERIPHERAL_CONFIG @ 0x24</div>
   <div><p>Tells you about the ao_peripheral config</p></div>
   <div>Reset default = 0xf, mask 0xf</div>
  </th>
 </tr>
<tr><td colspan=5><table class="regpic"><tr><td class="bitnum">31</td><td class="bitnum">30</td><td class="bitnum">29</td><td class="bitnum">28</td><td class="bitnum">27</td><td class="bitnum">26</td><td class="bitnum">25</td><td class="bitnum">24</td><td class="bitnum">23</td><td class="bitnum">22</td><td class="bitnum">21</td><td class="bitnum">20</td><td class="bitnum">19</td><td class="bitnum">18</td><td class="bitnum">17</td><td class="bitnum">16</td></tr><tr><td class="unused" colspan=16>&nbsp;</td>
</tr>
<tr><td class="bitnum">15</td><td class="bitnum">14</td><td class="bitnum">13</td><td class="bitnum">12</td><td class="bitnum">11</td><td class="bitnum">10</td><td class="bitnum">9</td><td class="bitnum">8</td><td class="bitnum">7</td><td class="bitnum">6</td><td class="bitnum">5</td><td class="bitnum">4</td><td class="bitnum">3</td><td class="bitnum">2</td><td class="bitnum">1</td><td class="bitnum">0</td></tr><tr><td class="unused" colspan=12>&nbsp;</td>
<td class="fname" colspan=1 style="font-size:42.857142857142854%">GPIO_AO</td>
<td class="fname" colspan=1 style="font-size:27.272727272727273%">PAD_CONTROL</td>
<td class="fname" colspan=1>DMA</td>
<td class="fname" colspan=1 style="font-size:33.333333333333336%">SPI_FLASH</td>
</tr></table></td></tr>
<tr><th width=5%>Bits</th><th width=5%>Type</th><th width=5%>Reset</th><th>Name</th><th>Description</th></tr><tr><td class="regbits">0</td><td class="regperm">ro</td><td class="regrv">0x1</td><td class="regfn">SPI_FLASH</td><td class="regde"><p>X-HEEP has SPI_FLASH</p></td><tr><td class="regbits">1</td><td class="regperm">ro</td><td class="regrv">0x1</td><td class="regfn">DMA</td><td class="regde"><p>X-HEEP has DMA</p></td><tr><td class="regbits">2</td><td class="regperm">ro</td><td class="regrv">0x1</td><td class="regfn">PAD_CONTROL</td><td class="regde"><p>X-HEEP has PAD_CONTROL</p></td><tr><td class="regbits">3</td><td class="regperm">ro</td><td class="regrv">0x1</td><td class="regfn">GPIO_AO</td><td class="regde"><p>X-HEEP has GPIO_AO</p></td></table>
<br>
<table class="regdef" id="Reg_xheep_peripheral_config">
 <tr>
  <th class="regdef" colspan=5>
   <div>soc_ctrl.XHEEP_PERIPHERAL_CONFIG @ 0x28</div>
   <div><p>Tells you about the peripheral config</p></div>
   <div>Reset default = 0x1bf, mask 0x1ff</div>
  </th>
 </tr>
<tr><td colspan=5><table class="regpic"><tr><td class="bitnum">31</td><td class="bitnum">30</td><td class="bitnum">29</td><td class="bitnum">28</td><td class="bitnum">27</td><td class="bitnum">26</td><td class="bitnum">25</td><td class="bitnum">24</td><td class="bitnum">23</td><td class="bitnum">22</td><td class="bitnum">21</td><td class="bitnum">20</td><td class="bitnum">19</td><td class="bitnum">18</td><td class="bitnum">17</td><td class="bitnum">16</td></tr><tr><td class="unused" colspan=16>&nbsp;</td>
</tr>
<tr><td class="bitnum">15</td><td class="bitnum">14</td><td class="bitnum">13</td><td class="bitnum">12</td><td class="bitnum">11</td><td class="bitnum">10</td><td class="bitnum">9</td><td class="bitnum">8</td><td class="bitnum">7</td><td class="bitnum">6</td><td class="bitnum">5</td><td class="bitnum">4</td><td class="bitnum">3</td><td class="bitnum">2</td><td class="bitnum">1</td><td class="bitnum">0</td></tr><tr><td class="unused" colspan=7>&nbsp;</td>
<td class="fname" colspan=1 style="font-size:75.0%">UART</td>
<td class="fname" colspan=1>I2S</td>
<td class="fname" colspan=1 style="font-size:42.857142857142854%">PDM2PCM</td>
<td class="fname" colspan=1 style="font-size:75.0%">SPI2</td>
<td class="fname" colspan=1 style="font-size:37.5%">RV_TIMER</td>
<td class="fname" colspan=1>I2C</td>
<td class="fname" colspan=1 style="font-size:75.0%">GPIO</td>
<td class="fname" colspan=1 style="font-size:37.5%">SPI_HOST</td>
<td class="fname" colspan=1 style="font-size:42.857142857142854%">RV_PLIC</td>
</tr></table></td></tr>
<tr><th width=5%>Bits</th><th width=5%>Type</th><th width=5%>Reset</th><th>Name</th><th>Description</th></tr><tr><td class="regbits">0</td><td class="regperm">ro</td><td class="regrv">0x1</td><td class="regfn">RV_PLIC</td><td class="regde"><p>X-HEEP has RV_PLIC</p></td><tr><td class="regbits">1</td><td class="regperm">ro</td><td class="regrv">0x1</td><td class="regfn">SPI_HOST</td><td class="regde"><p>X-HEEP has SPI_HOST</p></td><tr><td class="regbits">2</td><td class="regperm">ro</td><td class="regrv">0x1</td><td class="regfn">GPIO</td><td class="regde"><p>X-HEEP has GPIO</p></td><tr><td class="regbits">3</td><td class="regperm">ro</td><td class="regrv">0x1</td><td class="regfn">I2C</td><td class="regde"><p>X-HEEP has I2C</p></td><tr><td class="regbits">4</td><td class="regperm">ro</td><td class="regrv">0x1</td><td class="regfn">RV_TIMER</td><td class="regde"><p>X-HEEP has RV_TIMER</p></td><tr><td class="regbits">5</td><td class="regperm">ro</td><td class="regrv">0x1</td><td class="regfn">SPI2</td><td class="regde"><p>X-HEEP has SPI2</p></td><tr><td class="regbits">6</td><td class="regperm">ro</td><td class="regrv">0x0</td><td class="regfn">PDM2PCM</td><td class="regde"><p>X-HEEP has PDM2PCM</p></td><tr><td class="regbits">7</td><td class="regperm">ro</td><td class="regrv">0x1</td><td class="regfn">I2S</td><td class="regde"><p>X-HEEP has I2S</p></td><tr><td class="regbits">8</td><td class="regperm">ro</td><td class="regrv">0x1</td><td class="regfn">UART</td><td class="regde"><p>X-HEEP has UART</p></td></table>
<br>
