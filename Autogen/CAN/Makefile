# Commands
PERL = perl

# Directories
INC_DIR = Inc
SRC_DIR = Src
DOC_DIR = Doc

# Files
CANDO = $(DOC_DIR)/GRCAN.CANdo
STRUCT_ID = $(INC_DIR)/GRCAN_MSG_DATA.h
GR_ID = $(INC_DIR)/GRCAN_NODE_ID.h
MSG_ID = $(INC_DIR)/GRCAN_MSG_ID.h
CAN_ID = $(INC_DIR)/GRCAN_CUSTOM_ID.h
BUS_ID = $(INC_DIR)/GRCAN_BUS_ID.h

.PHONY: all clean STRUCTparser CANparser GRparser MSGparser BusParser

all: STRUCTparser CANparser GRparser MSGparser BusParser
	for file in $(STRUCT_ID) $(CAN_ID) $(GR_ID) $(MSG_ID) $(BUS_ID); do \
		if [ -f "$$file" ]; then \
			echo "Successfully generated $$file"; \
		else \
			echo "Error: $$file was not generated."; \
			exit 1; \
		fi; \
	done

clean:
	rm -f $(STRUCT_ID) $(CAN_ID) $(GR_ID) $(MSG_ID) $(BUS_ID)

STRUCTparser:
	$(PERL) $(SRC_DIR)/$(@).pl $(CANDO) $(STRUCT_ID)

CANparser:
	$(PERL) $(SRC_DIR)/$(@).pl $(CANDO) $(CAN_ID)

GRparser:
	$(PERL) $(SRC_DIR)/$(@).pl $(CANDO) $(GR_ID)

MSGparser:
	$(PERL) $(SRC_DIR)/$(@).pl $(CANDO) $(MSG_ID)

BusParser:
	$(PERL) $(SRC_DIR)/$(@).pl $(CANDO) $(BUS_ID)
