# Generate model resource file

## 0. Install modified TVM

## 1. Generate model using tvm

Using [tvm_generate_model.py](./tvm_generate_model.py) to generate `resnet_test.param`, `resnet_test.graph.json`, `resnet_test.raw_schedule.json`, `resnet_tes.cu`:

``` bash
# PWD == reef
mkdir resource/resnet_test
cd resource/resnet_test
python3 ../../script/tvm_generate_model.py \
        resnet_test.cu \
        resnet_test.raw_schedule.json \
        resnet_test.graph.json \
        resnet_test.param
```

## 2. Generate final final schedule

``` bash
python3 ../../script/generate_final_schedule.py \
        resnet_test.cu \
        resnet_test.raw_schedule.json \
        resnet_test.graph.json > resnet_test.json
rm resnet_test.graph.json resnet_test.raw_schedule.json
rmdir .pkl_memoize_py3
```

It will generate a schedule file.

## 3 Generate profile file

## 4 Hipcc compile

hipcc -o resnet_test.raw.co --genco --offload-arch=gfx1030 -O3 resnet_test.cu

