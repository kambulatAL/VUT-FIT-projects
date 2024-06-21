-- cpu.vhd: Simple 8-bit CPU (BrainFuck interpreter)
-- Copyright (C) 2022 Brno University of Technology,
--                    Faculty of Information Technology
-- Author(s): Alakaev Kambulat <xalaka00@stud.fit.vutbr.cz>
--
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;


-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity cpu is
 port (
   CLK   : in std_logic;  -- hodinovy signal
   RESET : in std_logic;  -- asynchronni reset procesoru
   EN    : in std_logic;  -- povoleni cinnosti procesoru
 
   -- synchronni pamet RAM
   DATA_ADDR  : out std_logic_vector(12 downto 0); -- adresa do pameti
   DATA_WDATA : out std_logic_vector(7 downto 0); -- mem[DATA_ADDR] <- DATA_WDATA pokud DATA_EN='1'
   DATA_RDATA : in std_logic_vector(7 downto 0);  -- DATA_RDATA <- ram[DATA_ADDR] pokud DATA_EN='1'
   DATA_RDWR  : out std_logic;                    -- cteni (0) / zapis (1)
   DATA_EN    : out std_logic;                    -- povoleni cinnosti
   
   -- vstupni port
   IN_DATA   : in std_logic_vector(7 downto 0);   -- IN_DATA <- stav klavesnice pokud IN_VLD='1' a IN_REQ='1'
   IN_VLD    : in std_logic;                      -- data platna
   IN_REQ    : out std_logic;                     -- pozadavek na vstup data
   
   -- vystupni port
   OUT_DATA : out  std_logic_vector(7 downto 0);  -- zapisovana data
   OUT_BUSY : in std_logic;                       -- LCD je zaneprazdnen (1), nelze zapisovat
   OUT_WE   : out std_logic                       -- LCD <- OUT_DATA pokud OUT_WE='1' a OUT_BUSY='0'
 );
end cpu;


-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of cpu is

 signal pc_reg  :std_logic_vector(11 downto 0);
 signal pc_inc  :std_logic;
 signal pc_dec  :std_logic;

 signal ptr_reg :std_logic_vector(11 downto 0);
 signal ptr_inc :std_logic;
 signal ptr_dec :std_logic;


 type inst_type is (
   inst_halt,         --zastav vykonavani programu
   inst_ptr_inc, -- inkrementace hodnoty ukazatele: ptr += 1;
   inst_ptr_dec, --  dekrementace hodnoty ukazatele: ptr -= 1;
   inst_val_inc, --  inkrementace hodnoty aktualni bunky: *ptr += 1;
   inst_val_dec, -- dekrementace hodnoty aktualni bunky: *ptr -= 1;
   inst_null_jump, -- je-li hodnota aktualni bunky nulova, skoc za odpovidajici prikaz ]
   inst_nonull_jump,-- je-li hodnota aktualni bunky nenulova, skoc za odpovidajici prikaz [
   inst_do_continue, -- zacatek cyklu do-while
   inst_dowhile_jump, -- je-li hodnota aktualni bunky nenulova, skoc za odpovidajici (, else -> pokracuj dale
   inst_print,       -- vytiskni hodnotu aktualni bunky
   inst_read,        --nacti hodnotu a uloz ji do aktualni bunky
   inst_unknown      -- komentar nebo cokoliv, co neni instrukce
 );
 signal inst_dec :inst_type;

 type fsm_state is (
   state_idle, 
   state_fetch,
   state_decode,
   state_ptr_inc,
   state_ptr_dec,
   state_val_inc1,
   state_val_inc2,
   state_val_dec1,
   state_val_dec2,
   state_null_jump1,
   state_null_jump2,
   state_null_jump3,
   state_nonull_jump1,
   state_nonull_jump2,
   state_nonull_jump3,
   state_do_continue,
   state_dowhile_jump1,
   state_dowhile_jump2,
   state_dowhile_jump3,
   state_halt,
   state_print1,
   state_print2,
   state_read,
   state_unknown
 );
 signal p_state  :fsm_state;
 signal n_state  :fsm_state;


 signal sel1     :std_logic;
 signal sel2     :std_logic_vector(1 downto 0);

begin

-- Programovy citac
 pc_cnt: process(RESET, CLK)
 begin
   if (RESET = '1') then
     pc_reg <= (others => '0');
   elsif rising_edge(CLK) then
     if (pc_inc = '1') then
       pc_reg <= pc_reg + 1;
     elsif (pc_dec = '1') then
       pc_reg <= pc_reg - 1;
     end if; 
   end if;
 end process pc_cnt;


 -- Ukazatel do pameti dat
 ptr_p: process(RESET,CLK)
 begin
   if (RESET = '1') then
     ptr_reg <= X"000";
   elsif rising_edge(CLK) then
     if (ptr_inc = '1') then
       ptr_reg <= ptr_reg + 1;
     elsif (ptr_dec = '1') then
       ptr_reg <= ptr_reg - 1;
     end if; 
   end if;
 end process ptr_p;


 -- Multiplexor pro urceni adresy programu/dat
 MX1: process(CLK,pc_reg,ptr_reg)
 begin
   if (sel1 = '1') then
     DATA_ADDR <= "1" & ptr_reg;
   elsif (sel1 = '0') then
     DATA_ADDR <= "0" &pc_reg;
   end if;
 end process MX1;


 --Multiplexor, ktery urcuje hodnotu zapisovanou do pameti
 MX2: process(CLK,sel2, DATA_RDATA, IN_DATA)
 begin
   case sel2 is 
     when "00" => DATA_WDATA <= IN_DATA;
     when "01" => DATA_WDATA <= DATA_RDATA - "00000001";
     when "10" => DATA_WDATA <= DATA_RDATA + "00000001";
     when others => DATA_WDATA <= X"00";
    
   end case;
 end process MX2;
 

 -- dekoder instrukci
 decoder: process(DATA_RDATA)
 begin
  

   case DATA_RDATA  is 
    when X"3E" =>inst_dec <= inst_ptr_inc; -- >
    when X"3C" =>inst_dec <= inst_ptr_dec; -- <
    when X"2B" =>inst_dec <= inst_val_inc; -- +
    when X"2D" =>inst_dec <= inst_val_dec; -- -
    when X"2E" =>inst_dec <= inst_print; -- .
    when X"2C" =>inst_dec <= inst_read; -- ,
    when X"28" =>inst_dec <= inst_do_continue; -- (
    when X"29" =>inst_dec <= inst_dowhile_jump; -- )
    when X"5B" =>inst_dec <= inst_null_jump; -- [
    when X"5D" =>inst_dec <= inst_nonull_jump; -- ]
    when X"00" =>inst_dec <= inst_halt; -- halt
     when others => inst_dec <= inst_unknown; -- neni instrukce
   end case;

 end process decoder;


-- aktualni stav fsm
 fsm_pstate: process(RESET,CLK)
 begin
   if (RESET = '1') then
     p_state <= state_idle;
   elsif (rising_edge(CLK)) and (EN = '1') then
     p_state <= n_state;
   end if;
 end process fsm_pstate;

-- nasledujici stav fsm 
 fsm_next_state: process(p_state,inst_dec,EN,DATA_RDATA,OUT_BUSY,IN_VLD,IN_DATA) 
 begin
   n_state   <= state_idle;
   DATA_EN   <= '0';
   DATA_RDWR <= '0';
   IN_REQ    <= '0';
   OUT_WE    <= '0';
   pc_inc    <= '0';
   pc_dec    <= '0';
   ptr_inc   <= '0';
   ptr_dec   <= '0';
   sel1      <= '0';
   sel2      <= "00";

   case p_state is 
     -- IDLE
     when state_idle => n_state <= state_fetch;

     --INSTRUCTION FETCH
     when state_fetch => 
       n_state <= state_decode;
       DATA_EN <= '1';
       sel1 <= '0';
     
     -- DECODE
     when state_decode =>
       case inst_dec is
         when inst_halt => n_state <= state_halt;
         when inst_ptr_inc => n_state <= state_ptr_inc;
         when inst_ptr_dec => n_state <= state_ptr_dec;
         when inst_val_inc => n_state <= state_val_inc1;
         when inst_val_dec => n_state <= state_val_dec1;
         when inst_null_jump => n_state <= state_null_jump1;
         when inst_nonull_jump => n_state <= state_nonull_jump1;
         when inst_do_continue => n_state <= state_do_continue;
         when inst_dowhile_jump => n_state <= state_dowhile_jump1;
         when inst_print => n_state <= state_print1;
         when inst_read => n_state <= state_read;
         when inst_unknown => n_state <= state_unknown;
       end case;

   when state_ptr_inc => -- inkrementace hodnoty ukazatele
       pc_inc <= '1'; -- nasledujici instrukce
       ptr_inc <= '1'; -- nasledujici bunka pameti
       n_state <= state_fetch;

   when state_ptr_dec => -- dekrementace hodnoty ukazatele:
       pc_inc <= '1'; -- nasledujici instrukce
       ptr_dec <= '1'; -- predchozi bunka pameti
       n_state <= state_fetch;
     
   when state_val_inc1 => -- cteme hodnotu, kterou cheme inkrementovat
      DATA_EN <= '1';
      sel1 <= '1';
      DATA_RDWR <= '0';
      n_state <= state_val_inc2;

   when state_val_inc2 => -- inkrementujeme prectenou hodnotu
      DATA_EN <= '1';
      DATA_RDWR <= '1';
      pc_inc <= '1';
      sel1 <= '1';
      sel2 <= "10";
      n_state <= state_fetch;
      
   when state_val_dec1 => --cteme hodnotu, kterou cheme dekrementovat
      DATA_EN <= '1';
      sel1 <= '1';
      DATA_RDWR <= '0';
      n_state <= state_val_dec2;
    
   when state_val_dec2 => -- dekrementujeme prectenou hodnotu
      DATA_EN <= '1';
      DATA_RDWR <= '1';
      pc_inc <= '1';
      sel1 <= '1';
      sel2 <= "01";
      n_state <= state_fetch;


  when state_print1 =>
        if (OUT_BUSY = '1') then
          n_state<= state_print1;
        else
        DATA_RDWR <= '0';
        DATA_EN <='1';
        sel1 <= '1';
        n_state <= state_print2;
        end if;
  when state_print2 => --vytisknout hodnotu v datove bunce    
        DATA_RDWR <= '0';
        DATA_EN <='1';
        sel1<='1';
        OUT_WE <= '1';
        pc_inc <= '1';
        OUT_DATA <= DATA_RDATA;
        n_state <= state_fetch;


  when state_read => --  precist hodnotu a ulozit v datovou bunku
      IN_REQ <= '1';
      sel1 <= '1';
      n_state <= state_read;
      if (IN_VLD= '1') then
        n_state <= state_fetch;
        DATA_EN <= '1';
        DATA_RDWR <= '1';
        pc_inc <= '1';
        sel2 <= "00";
        IN_REQ <= '0';
      end if;

  --  je-li hodnota aktualnı bunky nulova, skoc za odpovıdajici prikaz ]
  when state_null_jump1 =>
        pc_inc <= '1';
        DATA_EN <= '1';
        DATA_RDWR <= '0';
        sel1 <= '1';
        n_state <= state_null_jump2;

  when state_null_jump2 =>
        if (DATA_RDATA = "00000000") then
          sel1 <= '0';
          DATA_EN <= '1';
          DATA_RDWR <= '0';
          n_state <= state_null_jump3;
        else 
          n_state <= state_fetch;
        end if;

  when state_null_jump3 =>
          if (DATA_RDATA = "01011101") then --  01011101 je ']' v dvojkove soustave
            n_state <= state_fetch;
          else
            sel1 <= '0';
            DATA_EN <= '1';
            DATA_RDWR <= '0';
            n_state <= state_null_jump3;
          end if;
          pc_inc <= '1';

  --je-li hodnota aktualnı bunky nenulova, skoc za odpovıdajici prikaz [, jinak pokracujeme dale
  when state_nonull_jump1 =>
          DATA_EN <= '1';
          DATA_RDWR <= '0';
          sel1 <= '1';
          n_state <= state_nonull_jump2;

  when state_nonull_jump2 =>
          if (DATA_RDATA = "00000000") then
            pc_inc <= '1';
            n_state <= state_fetch;
          else
            DATA_EN <= '1';
            sel1 <= '0';
            DATA_RDWR <= '0';
            pc_dec <= '1';
            pc_inc <= '0';
            n_state <= state_nonull_jump3;
          end if;
  
  when state_nonull_jump3 =>
          if (DATA_RDATA = "01011011") then -- 01011011 je '[' v dvojkove soustave
            pc_inc <= '1';
            pc_dec <= '0';
            n_state <= state_fetch;
          else 
            pc_dec <= '1';
            sel1 <= '0';
            DATA_EN <= '1';
            DATA_RDWR <= '0';
            n_state <= state_nonull_jump3;
          end if;
  
 
 -- do while(zacatek): proste pokracujeme dal
  when state_do_continue =>
          pc_inc <= '1';
          n_state <= state_fetch;
  
  --je-li hodnota aktualnı bunky nenulova, skoc za odpovıdajici prikaz (, jinak pokracujeme dale
  when state_dowhile_jump1 =>
          DATA_EN <= '1';
          DATA_RDWR <= '0';
          sel1 <= '1';
          n_state <= state_dowhile_jump2;
  
  when state_dowhile_jump2 =>
          if (DATA_RDATA = "00000000") then
            pc_inc <= '1';
            n_state <= state_fetch;
          else
            DATA_EN <= '1';
            sel1 <= '0';
            DATA_RDWR <= '0';
            pc_dec <= '1';
            --pc_inc <= '0';
            n_state <= state_dowhile_jump3;
          end if;

  when state_dowhile_jump3 =>
          if (DATA_RDATA = "00101000") then -- 00101000 je '(' v dvojkove soustave
            pc_inc <= '1';
            pc_dec <= '0';
            n_state <= state_fetch;
          else 
            pc_dec <= '1';
            sel1 <= '0';
            DATA_EN <= '1';
            DATA_RDWR <= '0';
            n_state <= state_dowhile_jump3;
          end if;

  -- HALT   
  when state_halt => n_state <= state_halt;

          
  -- neni instrukce
  when state_unknown => 
      n_state <= state_fetch;
      pc_inc <= '1';

   end case;
 end process fsm_next_state;
end behavioral;

