# Copy data from AliEn to ALIKE EOS

## Create AliEn token (it is valid for 24 hours)

```
$ alien-token-init
```

## Create list of files to be downloaded

This example will show LHC15f pass2 data for run 225000
```
aliensh
aliensh:[alice] [2] /alice/cern.ch/user/m/mvala/ > find  /alice/data/2015/LHC15f/000225000/pass2 AliESDs.root > LHC15f_pass2_225000.txt
```

## Download files
```
${ALI_RSN_ANALYSUS_PROJECT_DIR}/scripts/alice_cp.sh LHC15f_pass2_225000.txt
```
