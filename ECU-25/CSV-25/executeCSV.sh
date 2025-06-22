#!/bin/sh
set -e;

if test -d CSV-25
then
    python3 CSV-25/grCSVtoH.py CSV-25/GR-ID.csv Application/Inc/grIDs.h
    python3 CSV-25/msgCSVtoH.py CSV-25/MSG-ID.csv Application/Inc/msgIDs.h
    python3 CSV-25/customIdCSVtoH.py CSV-25/CUSTOM-ID.csv Application/Inc/customIDs.h

    echo "-----------"
    echo "  Heads up!"
    echo "  Due to security limitations you will need to confirm our local CSVs match the online version"
    echo "  at https://docs.google.com/spreadsheets/d/1XfJhhAQoDnuSuwluNitPsDWtuQu-bP-VbEGPmSo5ujA/edit?usp=sharing"
    echo "  If out of date, update the CSV in ./CSV-25/ and rebuild to update the header files"
    echo "-----------"
    sleep 2
    
else
    echo "CSV-25 repository not found!"
    exit 1
fi

exit 0
