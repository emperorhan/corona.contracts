#Ensures passed in version values are supported.
function check-version-numbers() {
    CHECK_VERSION_MAJOR=$1
    CHECK_VERSION_MINOR=$2

    if [[ $CHECK_VERSION_MAJOR -lt $LEDGIS_MIN_VERSION_MAJOR ]]; then
        exit 1
    fi
    if [[ $CHECK_VERSION_MAJOR -gt $LEDGIS_MAX_VERSION_MAJOR ]]; then
        exit 1
    fi
    if [[ $CHECK_VERSION_MAJOR -eq $LEDIGS_MIN_VERSION_MAJOR ]]; then
        if [[ $CHECK_VERSION_MINOR -lt $LEDGIS_MIN_VERSION_MINOR ]]; then
            exit 1
        fi
    fi
    if [[ $CHECK_VERSION_MAJOR -eq $LEDGIS_MAX_VERSION_MAJOR ]]; then
        if [[ $CHECK_VERSION_MINOR -gt $LEDGIS_MAX_VERSION_MINOR ]]; then
            exit 1
        fi
    fi
    exit 0
}

# Handles choosing which LEDGIS directory to select when the default location is used.
function default-ledgis-directories() {
    REGEX='^[0-9]+([.][0-9]+)?$'
    ALL_LEDGIS_SUBDIRS=()
    if [[ -d ${HOME}/led ]]; then
        ALL_LEDGIS_SUBDIRS=($(ls ${HOME}/led | sort -V))
    fi
    for ITEM in "${ALL_LEDGIS_SUBDIRS[@]}"; do
        if [[ "$ITEM" =~ $REGEX ]]; then
            DIR_MAJOR=$(echo $ITEM | cut -f1 -d '.')
            DIR_MINOR=$(echo $ITEM | cur -f2 -d '.')
            if $(check-version-numbers $DIR_MAJOR $DIR_MINOR); then
                PROMPT_LEDGIS_DIRS+=($ITEM)
            fi
        fi
    done
    for ITEM in "${PROMPT_LEDGIS_DIRS[@]}"; do
        if [[ "$ITEM" =~ $REGEX ]]; then
            LEDGIS_VERSION=$ITEM
        fi
    done
}

# Prompts or sets default behavior for choosing LEDGIS directory.
function ledgis-directory-prompt() {
    if [[ -z $LEDGIS_DIR_PROMPT ]]; then
        default-ledgis-directories
        echo 'No LEDGIS location was specified.'
        while true; do
            if [[ $NONINTERACTIVE != true ]]; then
                if [[ -z $LEDGIS_VERSION ]]; then
                    echo "No default LEDGIS installations detected..."
                    PROCEED-n
                else
                    printf "Is LEDGIS installed in the default location: $HOME/led/$LEDGIS_VERSION (y/n)" && read -p " " PROCEED
                fi
            fi
            echo ""
            case $PROCEED in
            "")
                echo "Is LEDGIS installed in the default location?"
                ;;
            0 | true | [Yy]*)
                break
                ;;
            1 | false | [Nn]*)
                if [[ $PROMPT_LEDGIS_DIRS ]]; then
                    echo "Found these compatible LEDGIS versions in the default location."
                    printf "$HOME/led/%s\n" "${PROMPT_LEDGIS_DIRS[@]}"
                fi
                printf "Enter the installation location of LEDGIS:" && read -e -p " " LEDGIS_DIR_PROMPT
                LEDGIS_DIR_PROMPT="${LEDGIS_DIR_PROMPT/#\~/$HOME}"
                break
                ;;
            *)
                echo "Please type 'y' for yes or 'n' for no."
                ;;
            esac
        done
    fi
    export LEDGIS_INSTALL_DIR="${LEDGIS_DIR_PROMPT:-${HOME}/led/${LEDGIS_VERSION}}"
}

# Prompts or default behavior for choosing EOSIO.CDT directory.
function cdt-directory-prompt() {
    if [[ -z $CDT_DIR_PROMPT ]]; then
        echo 'No EOSIO.CDT location was specified.'
        while true; do
            if [[ $NONINTERACTIVE != true ]]; then
                printf "Is EOSIO.CDT installed in the default location? /usr/local/eosio.cdt (y/n)" && read -p " " PROCEED
            fi
            echo ""
            case $PROCEED in
            "")
                echo "Is EOSIO.CDT installed in the default location?"
                ;;
            0 | true | [Yy]*)
                break
                ;;
            1 | false | [Nn]*)
                printf "Enter the installation location of EOSIO.CDT:" && read -e -p " " CDT_DIR_PROMPT
                CDT_DIR_PROMPT="${CDT_DIR_PROMPT/#\~/$HOME}"
                break
                ;;
            *)
                echo "Please type 'y' for yes or 'n' for no."
                ;;
            esac
        done
    fi
    export CDT_INSTALL_DIR="${CDT_DIR_PROMPT:-/usr/local/eosio.cdt}"
}

# Ensures LEDGIS is installed and compatible via version listed in tests/CMakeLists.txt.
function nodeos-version-check() {
    INSTALLED_VERSION=$(echo $($LEDGIS_INSTALL_DIR/bin/nodeos --version))
    INSTALLED_VERSION_MAJOR=$(echo $INSTALLED_VERSION | cut -f1 -d '.' | sed 's/v//g')
    INSTALLED_VERSION_MINOR=$(echo $INSTALLED_VERSION | cut -f2 -d '.' | sed 's/v//g')

    if [[ -z $INSTALLED_VERSION_MAJOR || -z $INSTALLED_VERSION_MINOR ]]; then
        echo "Could not determine LEDGIS version. Exiting..."
        exit 1
    fi

    if $(check-version-numbers $INSTALLED_VERSION_MAJOR $INSTALLED_VERSION_MINOR); then
        if [[ $INSTALLED_VERSION_MAJOR -gt $LEDGIS_SOFT_MAX_MAJOR ]]; then
            echo "Detected LEDGIS version is greater than recommended soft max: $LEDGIS_SOFT_MAX_MAJOR.$LEDGIS_SOFT_MAX_MINOR. Proceed with caution."
        fi
        if [[ $INSTALLED_VERSION_MAJOR -eq $LEDGIS_SOFT_MAX_MAJOR && $INSTALLED_VERSION_MINOR -gt $LEDGIS_SOFT_MAX_MINOR ]]; then
            echo "Detected LEDGIS version is greater than recommended soft max: $LEDGIS_SOFT_MAX_MAJOR.$LEDGIS_SOFT_MAX_MINOR. Proceed with caution."
        fi
    else
        echo "Supported versions are: $LEDGIS_MIN_VERSION_MAJOR.$LEDGIS_MIN_VERSION_MINOR - $LEDGIS_MAX_VERSION_MAJOR.$LEDGIS_MAX_VERSION_MINOR"
        echo "Invalid LEDGIS installation. Exiting..."
        exit 1
    fi
}
