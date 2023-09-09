# A clean start is always good. 
# do not use -all, that one will delete all std cell libraries
remove_design -designs 

# read library
set target_library \ {../LIB/cmos22nm.db}
set link_library \ {../LIB/cmos22nm.db}

# analyze the design

#*******************************************

analyze -f vhdl ../SRC/components/adder.vhd
analyze -f vhdl ../SRC/components/mul.vhd
analyze -f vhdl ../SRC/components/reg.vhd
analyze -f vhdl ../SRC/components/array_t.vhd

#*******************************************

analyze -f vhdl ../SRC/fir_sol/fir_sol.vhd
analyze -f vhdl ../SRC/fir_sol/fir_sol_wrapper.vhd

# elaborate the design 
elaborate fir_sol_wrapper

# set delay constraints
create_clock "Clk" -name "global_clk" -period 1.75

# now let's just compile and see what comes up 
compile

# get some useful report about area and timing   
#report_qor
report_area
report_timing
# quit design compiler
exit
