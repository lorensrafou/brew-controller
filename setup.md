# Setting up dotnetcore server

## Installing Pre-Requisites on Rapsberry Pi

Pre-requisite packages:
```
sudo apt-get install curl libunwind8 gettext
```

Download the .NET Core 3.0 preview 6 SDK:
```
wget https://download.visualstudio.microsoft.com/download/pr/50bc5936-b374-490b-9312-f3ca23c0bcfa/d7680c7a396b115d95ac835334777d02/dotnet-sdk-3.0.100-preview6-012264-linux-arm.tar.gz
```

Untar and set path for the .NET Core:
```
mkdir -p $HOME/dotnet && tar zxf dotnet-sdk-3.0.100-preview6-012264-linux-arm.tar.gz -C $HOME/dotnet
export DOTNET_ROOT=$HOME/dotnet 
export PATH=$PATH:$HOME/dotnet
```
