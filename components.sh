#!/bin/bash

git clone https://github.com/olikraus/u8g2.git components/u8g2

echo "COMPONENT_SRCDIRS:=csrc\nCOMPONENT_ADD_INCLUDEDIRS:=csrc" > components/u8g2/component.mk

git clone https://github.com/mauriciobarroso/sht4x.git components/sht4x
