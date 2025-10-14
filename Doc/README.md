# Self-Hosted Runner for Execution Test

The CMSIS-Toolbox implements and
[Run and Debug Configuration](https://open-cmsis-pack.github.io/cmsis-toolbox/build-overview/#run-and-debug-configuration)
for command line usage with pyOCD. pyOCD is a debug connector used in [Keil Studio] that offers also command-line
operation for Continuous Integration (CI).

This section explains how to setup a Linux box that runs a GitHub self-hosted runner for programming and execution of
an application. The [build workflow](../.github/workflows/Build_NUCLEO_H563ZI_Release.yaml) is executed on a GitHub
hosted runner that stores the build output as an artifact.

![CI and HiL Test](CI_HIL.png "CI and HiL Test")

## Prepare a self-hosted runner

These installation instructions are written for a Raspberry Pi (mode 3/4/5) running an Arm64 Ubuntu server edition.
They have been tested on [Ubuntu Server 24.0.3 LTS](https://ubuntu.com/download/server). For other Linux OS, please
adapt the steps.

## Prerequisites

The following commands need to be run to have all the required software installed on the local machine.

```sh
# Make sure your system is up-to-date
sudo apt update && sudo apt upgrade

# Install required tools
sudo apt install net-tools zip unzip
```

## Install pyOCD

> [!NOTE]
> Ubuntu Server 24.0.3 already comes with Python 3.12.x which is used in the following to describe the set up of
> `pyocd` in a virtual environment. Please make sure to install an appropriate Python version on your system as well.

```sh
# Create a virtual environment
python -m venv ~/pyocd_venv

# Activate the virtual environment
source ~/pyocd_venv/bin/activate

# Install pyocd
pip3 install pyocd
```

## vcpkg

vcpkg is a free C/C++ package manager for acquiring and managing libraries and is used in the CMSIS solution workflow
to download additional tools.

### Installation

Download `vcpkg` from the GitHub repository:

```sh
wget -qO vcpkg.tar.gz https://github.com/microsoft/vcpkg/archive/master.tar.gz
```

Create a new directory to store `vcpkg` and extract the tar.gz file to it:

```sh
sudo mkdir .vcpkg
sudo tar xf vcpkg.tar.gz --strip-components=1 -C .vcpkg
```

Run the following command to build vcpkg itself:

```sh
sudo .vcpkg/bootstrap-vcpkg.sh
```

> [!NOTE]
> If `node` is not installed on your machine, it will be downloaded automatically.

Export the path to `vcpkg` to your `$PATH` environment variable:

```sh
export PATH=/home/user/.vcpkg:$PATH
```

Now, `vcpkg` can be used. Check `vcpkg` version with command:

```sh
vcpkg version
```

The tar.gz file is no longer needed, remove it:

```sh
rm -rf vcpkg.tar.gz
```

### Activation

Download the `vcpkg-configuration.json` file that contains the CMSIS-Toolbox:

```sh
curl -o vcpkg-configuration.json -L https://raw.githubusercontent.com/Arm-Examples/Safety-Example-STM32/refs/heads/main/vcpkg-run-configuration.json
```

Then, update the registry

```sh
vcpkg x-update-registry --all
```

Finally, activate vcpkg:

```sh
vcpkg-shell activate --json=vcpkg-configuration.json
```

## Install the required CMSIS-Packs

Now, you have access to the CMSIS-Toolbox which also contains the `cpackget` tool which is used to download CMSIS-Packs
from the web. For this example, we need the device family and board support packs from Infineon.

```sh
cpackget add -a -F Keil::STM32H5xx_DFP@2.1.0
cpackget add -a -F Keil::NUCLEO-H563ZI_BSP@1.1.0
```

> [!NOTE]
> Run this command again if the pack versions change.

## Install the self-hosted runner

### Download and extract the runner

In GitHub → repo or org → Settings → Actions → Runners → New self-hosted runner → Linux → ARM/ARM64. You’ll see a
download link and a registration token. On the Pi:

```sh
mkdir ~/actions-runner && cd ~/actions-runner
curl -o actions-runner-linux-arm64.tar.gz -L https://github.com/actions/runner/releases/download/vX.Y.Z/actions-runner-linux-arm64-X.Y.Z.tar.gz
tar xzf ./actions-runner-linux-arm64.tar.gz
```

#### Install dependencies

```sh
sudo ./bin/installdependencies.sh
```

### Configure the runner

Use the URL and registration token from the GitHub UI:

```sh
./config.sh \
  --url https://github.com/<OWNER>/<REPO_OR_ORG> \
  --token <TOKEN> \
  --name rpi
```

### Run persistently

Install a `systemd` service that starts automatically at boot:

```sh
sudo ./svc.sh install
```

Start the service manually:

```sh
sudo ./svc.sh start
```

Check the status of the service:

```sh
sudo ./svc.sh status
```
