ifdef CMBSDIR 
CMBSBASE:=$(CMBSDIR)/cmbs
includes += -I$(CMBSDIR)/include 
else
CMBSBASE:=$(PROJDIR)/cmbs
endif

includes += -I$(CMBSBASE)
cmbs_objects:=
####################################################################
# settle includes

#includes += -I$(FRAMEBASE) -I$(FRAMEBASE)/vone 
####################################################################
# settle objects

ifdef CMBS_HOST
OBJ_EXT = o
else
OBJ_EXT = otwi
endif

cmbs_objects += $(OBJDIR)/cmbs_int.$(OBJ_EXT)
cmbs_objects += $(OBJDIR)/cmbs_api.$(OBJ_EXT)
cmbs_objects += $(OBJDIR)/cmbs_dcm.$(OBJ_EXT)
cmbs_objects += $(OBJDIR)/cmbs_dsr.$(OBJ_EXT)
cmbs_objects += $(OBJDIR)/cmbs_dee.$(OBJ_EXT)
cmbs_objects += $(OBJDIR)/cmbs_dem.$(OBJ_EXT)
cmbs_objects += $(OBJDIR)/cmbs_ie.$(OBJ_EXT)
cmbs_objects += $(OBJDIR)/cmbs_cmd.$(OBJ_EXT)
cmbs_objects += $(OBJDIR)/cmbs_suota.$(OBJ_EXT)
cmbs_objects += $(OBJDIR)/cmbs_rtp.$(OBJ_EXT)
cmbs_objects += $(OBJDIR)/cmbs_util.$(OBJ_EXT)
cmbs_objects += $(OBJDIR)/cmbs_fifo.$(OBJ_EXT)
cmbs_objects += $(OBJDIR)/cmbs_dbg.$(OBJ_EXT)
cmbs_objects += $(OBJDIR)/cmbs_han.$(OBJ_EXT)
cmbs_objects += $(OBJDIR)/cmbs_han_ie.$(OBJ_EXT)

####################################################################
# settle vpath

vpath %.c 	$(CMBSBASE)
