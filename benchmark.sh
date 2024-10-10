#!/bin/bash

data1="Lorem ipsum dolor sit amet, consectetur adipiscing elit."
data2="Interdum et malesuada fames ac ante ipsum primis in faucibus."
data3="Nunc pharetra elit magna, eu accumsan lorem convallis et."

for (( i=0; i<10; i++ )); do  
  ringUUID=$(./blockring newring)
  echo "New Ring: ${ringUUID}"
  blockUUID=$(./blockring newblock ${ringUUID} "${data1}")
  #echo "New Block: ${blockUUID}"
  blockUUID=$(./blockring newblock ${ringUUID} "${data2}")
  #echo "New Block: ${blockUUID}"
  blockUUID=$(./blockring newblock ${ringUUID} "${data3}")
  #echo "New Block: ${blockUUID}"
  ringJSON=$(./blockring closering ${ringUUID})
done