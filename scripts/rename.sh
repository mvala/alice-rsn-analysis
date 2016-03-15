#!/bin/bash

trap '{ echo "Hey, you pressed Ctrl-C.  Time to quit." ; exit 1; }' INT
PROJECT_DIR="$(dirname $(dirname $(readlink -m $0)))"
export EOS_MGM_URL="root://alieos.saske.sk"

# MC
ARGS_EXTRA=',"RsnOut_tpc3s","phippMC_100_phi_"'
# DATA
#ARGS_EXTRA=',"RsnOut_tpc3s","phippData_100_phi_"'

for f in $(eos -b find -f /eos/alike.saske.sk/alice/sim/2015/LHC15g3a3 | grep 376_20160308-1500);do
  echo "Doing '$f' ..."
  fout=${f//\/eos\/alike.saske.sk\/alice/\/eos\/alike.saske.sk\/alice\/alike/}
  fout=${fout//root_archive.zip/}
  eos -b mkdir -p $fout
  root -b -q $PROJECT_DIR/macros/analysis/'RsnRename.C("root://alieos.saske.sk/'$f'#AnalysisResults.root","root://alieos.saske.sk/'$fout'AnalysisResults.root"'$ARGS_EXTRA')'
done

