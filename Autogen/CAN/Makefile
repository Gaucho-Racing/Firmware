# Commands
PERL = perl

# Directories
INC_DIR = Inc
SRC_DIR = Src
DOC_DIR = Doc

# Files
CANDO = $(DOC_DIR)/format.CANdo
STRUCT_ID = $(INC_DIR)/Struct_ID.h
GR_ID = $(INC_DIR)/GR_IDS.h
MSG_ID = $(INC_DIR)/CAN_msg_ID.h
CAN_ID = $(INC_DIR)/Custom_CAN_ID.h
DBC = $(DOC_DIR)/output.dbc

.PHONY: all clean STRUCTparser CANparser DBCparser GRparser MSGparser

all: STRUCTparser CANparser DBCparser GRparser MSGparser
	for file in $(STRUCT_ID) $(CAN_ID) $(DBC) $(GR_ID) $(MSG_ID); do \
		if [ -f "$$file" ]; then \
			echo "Successfully generated $$file"; \
		else \
			echo "Error: $$file was not generated."; \
			exit 1; \
		fi; \
	done

clean:
	rm -f $(STRUCT_ID) $(CAN_ID) $(DBC) $(GR_ID) $(MSG_ID)

STRUCTparser:
	$(PERL) $(SRC_DIR)/$(@).pl $(CANDO) $(STRUCT_ID)

CANparser:
	$(PERL) $(SRC_DIR)/$(@).pl $(CANDO) $(CAN_ID)

DBCparser:
	$(PERL) $(SRC_DIR)/$(@).pl $(CANDO) $(DBC)

GRparser:
	$(PERL) $(SRC_DIR)/$(@).pl $(CANDO) $(GR_ID)

MSGparser:
	$(PERL) $(SRC_DIR)/$(@).pl $(CANDO) $(MSG_ID)
