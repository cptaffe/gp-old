#!/bin/bash

grep node node*.outExploreSeededSearchSpace.log | cut -d":" -f2 | sort -t, -k4,4

##################################################################################
exit


