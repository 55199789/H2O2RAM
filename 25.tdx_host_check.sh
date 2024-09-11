      
#!/bin/bash

set -u
set -o pipefail

declare -r _this_script_=$(basename $0)

function FAIL()
{
    local ts=$(date '+%Y-%m-%d %H:%M:%S')
    echo -e "\e[0;37;41m[TDX|FAIL|${ts}]\e[0m ${@}"
    exit 1
}

function WARN()
{
    local ts=$(date '+%Y-%m-%d %H:%M:%S')
    echo -e "\e[0;37;43m[TDX|WARN|${ts}]\e[0m ${@}"
}

function NOTE()
{
    local ts=$(date '+%Y-%m-%d %H:%M:%S')
    echo -e "\e[0;37;46m[TDX|NOTE|${ts}]\e[0m ${@}"
}

function INFO()
{
    local ts=$(date '+%Y-%m-%d %H:%M:%S')
    echo "[TDX|INFO|${ts}] ${@}"
}

function install_pkgs()
{
    local pkgs_need_install=""

    if ! command -v "cpuid" >/dev/null 2>&1; then
        pkgs_need_install+="cpuid "
    fi

    if ! command -v "rdmsr" >/dev/null 2>&1; then
        pkgs_need_install+="msr-tools "
    fi

    if [ -n "${pkgs_need_install}" ]; then
        local os_release=$(grep -E '^ID=\"?[0-9a-zA-Z]+\"?' /etc/os-release | tr -d '"' | awk -F'=' '{ print $NF }')

        case ${os_release} in
        rhel|rocky|centos)
            dnf install -y ${pkgs_need_install}
            ;;

        debian|ubuntu)
            apt install -y ${pkgs_need_install}
            ;;
        esac
    fi
}


function print_msr_check_result()
{
    if [ $# -eq 0 ]; then
        printf "%s\n" "---------------------------------------------------------------------"
        printf "%-40s%-8s%-64s\n" "^Msr[bits]" "^Value" "^Description"
    else
        local msr=$1
        local val=$2
        local msg=$3

        printf " %-40s%-8s%-64s\n" "${msr}" "${val}" "${msg}"
    fi
}


function tdx_host_msr_check()
{
    print_msr_check_result

    # SEAMRR
    print_msr_check_result  "IA32_SEAMRR_PHYS_MASK[11]"             $(rdmsr 0x1401 -f 11:11)  "Enable bit for the SEAMRR"

    # TME
    print_msr_check_result  "IA32_TME_CAPABILITY[0]"                $(rdmsr -f 0:0 0x981)     "When set to 1, AES-XTS 128-bit encryption algorithm supported (NIST standard)"
    print_msr_check_result  "IA32_TME_CAPABILITY[1]"                $(rdmsr -f 1:1 0x981)     "When set to 1, AES-XTS 128-bit encryption with at least 28-bits of integrity algorithm supported"
    print_msr_check_result  "IA32_TME_CAPABILITY[2]"                $(rdmsr -f 2:2 0x981)     "When set to 1, AES-XTS 256-bit encryption algorithm supported (NIST standard)"
    print_msr_check_result  "IA32_TME_CAPABILITY[35:32]"            $(rdmsr -f 35:32 0x981)   "MK_TME_MAX_KEYID_BITS, Number of bits which can be allocated for usage as key identifiers for MK-TME"
    print_msr_check_result  "IA32_TME_CAPABILITY[50:36]"            $(rdmsr -f 50:36 0x981)   "MK_TME_MAX_KEYS, Indicates the maximum number of keys which are available for usage"

    print_msr_check_result  "IA32_TME_ACTIVATE[1]"                  $(rdmsr -f 1:1 0x982)     "Hardware Encryption Enable (This bit also enables TME-MK)"
    print_msr_check_result  "IA32_TME_ACTIVATE[31]"                 $(rdmsr -f 31:31 0x982)   "TME Encryption Bypass Enable"
    print_msr_check_result  "IA32_TME_ACTIVATE[35:32]"              $(rdmsr -f 35:32 0x982)   "MK_TME_KEYID_BITS, The number of key identifier bits to allocate to TME-MK usage"
    print_msr_check_result  "IA32_TME_ACTIVATE[39:36]"              $(rdmsr -f 39:36 0x982)   "TDX_RESERVED_KEYID_BITS, Physical address bits starting at (MAXPHYADDR - 1) that are reserved for TDX private KeyID."
    print_msr_check_result  "IA32_TME_ACTIVATE[48]"                 $(rdmsr -f 48:48 0x982)   "MK_TME_CRYPTO_ALGS: AES-XTS 128"
    print_msr_check_result  "IA32_TME_ACTIVATE[49]"                 $(rdmsr -f 50:50 0x982)   "MK_TME_CRYPTO_ALGS: AES-XTS-256"

    print_msr_check_result  "IA32_MKTME_KEYID_PARTITIONING[31:0]"   $(rdmsr -f 31:0 0x87)     "NUM_MKTME_KIDS, Total Number of activated MKTME KeyIDs, [1, NUM_MKTME_KIDS]"
    print_msr_check_result  "IA32_MKTME_KEYID_PARTITIONING[63:32]"  $(rdmsr -f 63:32 0x87)    "NUM_TDX_PRIV_KIDS, Number of TDX Private KeyIDs, [NUM_MKTME_KIDS + 1, NUM_MKTME_KIDS + NUM_TDX_PRIV_KIDS]"

    echo ""
}


function tdx_host_check_sgx_using_cpuid()
{
    printf "%s\n" "---------------------------------------------------------------------"
    cpuid -1 | grep -i --color=always 'sgx'

    echo ""
}


function tdx_host_check_tdx_module_version()
{
    printf "%s\n" "---------------------------------------------------------------------"
    grep --color=always . /sys/firmware/tdx/tdx_module/*

    echo ""
}


function tdx_host_check_dmesg()
{
    printf "%s\n" "---------------------------------------------------------------------"
    dmesg | grep -i --color=always 'tdx'

    echo ""
}

function main()
{
    printf "%s\n" "---------------------------------------------------------------------"
    tdx_host_msr_check
    tdx_host_check_sgx_using_cpuid
    tdx_host_check_tdx_module_version
    tdx_host_check_dmesg

    echo ""
}

main

    