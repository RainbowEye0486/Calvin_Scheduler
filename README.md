# DevOps framework


## install

**Step1** : build a docker image for this project.
```shell
$docker build -t devops_template .
```
you can use command 
```shell
$docker images
```
to check if there is a images names `devops_template` in your images list.
execute run command to run up your container , name it as `devOpsDemo` or anything you name it.
```shell
$docker run -it --name devOpsDemo devops_template
```

if everything goes well , you shall see your terminal likes
```shell
root@c845cd69fad7:/devOps# 
```
where `[c845cd69fad7]` is your container ID .

**Step2** : build project
before your any operation, be sure to get update with latest stable version.
then build this project.

```shell
mkdir build
cd build
cmake ..
make
```