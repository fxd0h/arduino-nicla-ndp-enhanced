#!/bin/bash
# Arduino Nicla Voice Libraries Installation Script
# Cross-platform installation for Linux, macOS, and Windows (WSL/Git Bash)

set -e

# Detect operating system
detect_os() {
    if [[ "$OSTYPE" == "darwin"* ]]; then
        OS="macos"
    elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
        OS="linux"
    elif [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "cygwin" ]] || [[ -n "$WINDIR" ]]; then
        OS="windows"
    else
        OS="unknown"
    fi
}

# Color codes for output (cross-platform)
if [[ "$OS" == "windows" ]]; then
    # Windows doesn't support colors in some terminals
    RED=''
    GREEN=''
    YELLOW=''
    BLUE=''
    PURPLE=''
    CYAN=''
    NC=''
else
    RED='\033[0;31m'
    GREEN='\033[0;32m'
    YELLOW='\033[1;33m'
    BLUE='\033[0;34m'
    PURPLE='\033[0;35m'
    CYAN='\033[0;36m'
    NC='\033[0m'
fi

# Configuration based on OS
configure_paths() {
    case $OS in
        "macos")
            ARDUINO_LIBRARIES_DIR="$HOME/Library/Arduino15/packages/arduino/hardware/mbed_nicla/4.4.1/libraries"
            ARDUINO_EXAMPLES_DIR="$HOME/Arduino/libraries/NiclaVoice-Examples"
            ;;
        "linux")
            ARDUINO_LIBRARIES_DIR="$HOME/.arduino15/packages/arduino/hardware/mbed_nicla/4.4.1/libraries"
            ARDUINO_EXAMPLES_DIR="$HOME/Arduino/libraries/NiclaVoice-Examples"
            ;;
        "windows")
            ARDUINO_LIBRARIES_DIR="$USERPROFILE/AppData/Local/Arduino15/packages/arduino/hardware/mbed_nicla/4.4.1/libraries"
            ARDUINO_EXAMPLES_DIR="$USERPROFILE/Documents/Arduino/libraries/NiclaVoice-Examples"
            ;;
    esac
    
    BACKUP_DIR="$HOME/arduino-nicla-backup-$(date +%Y%m%d_%H%M%S)"
}

# Initialize
detect_os
configure_paths

echo -e "${BLUE}Arduino Nicla Voice Libraries Installation${NC}"
echo "=================================================="
echo -e "${CYAN}This script will install the modified NDP library for Nicla Voice${NC}"
echo -e "${CYAN}Detected OS: $OS${NC}"
echo ""

# Function to print status
print_status() {
    local status=$1
    local message=$2
    case $status in
        "info")
            echo -e "${BLUE}ℹ️  $message${NC}"
            ;;
        "success")
            echo -e "${GREEN}✅ $message${NC}"
            ;;
        "warning")
            echo -e "${YELLOW}⚠️  $message${NC}"
            ;;
        "error")
            echo -e "${RED}❌ $message${NC}"
            ;;
        "step")
            echo -e "${PURPLE}🔧 $message${NC}"
            ;;
    esac
}

# Function to check if directory exists
dir_exists() {
    [ -d "$1" ]
}

# Function to check if file exists
file_exists() {
    [ -f "$1" ]
}

# Function to create backup
create_backup() {
    print_status "step" "Creating backup of existing NDP library..."
    
    if dir_exists "$ARDUINO_LIBRARIES_DIR/NDP"; then
        print_status "info" "Backing up existing NDP library to $BACKUP_DIR"
        mkdir -p "$BACKUP_DIR"
        cp -r "$ARDUINO_LIBRARIES_DIR/NDP" "$BACKUP_DIR/"
        print_status "success" "Backup created successfully"
    else
        print_status "info" "No existing NDP library found, skipping backup"
    fi
}

# Function to install NDP library
install_ndp_library() {
    print_status "step" "Installing modified NDP library..."
    
    # Check if Arduino libraries directory exists
    if ! dir_exists "$ARDUINO_LIBRARIES_DIR"; then
        print_status "error" "Arduino libraries directory not found: $ARDUINO_LIBRARIES_DIR"
        print_status "info" "Please install Arduino IDE and Nicla Voice board support first"
        print_status "info" "Run: arduino-cli core install arduino:mbed_nicla"
        return 1
    fi
    
    # Remove existing NDP library
    if dir_exists "$ARDUINO_LIBRARIES_DIR/NDP"; then
        print_status "info" "Removing existing NDP library..."
        rm -rf "$ARDUINO_LIBRARIES_DIR/NDP"
    fi
    
    # Install new NDP library
    print_status "info" "Installing modified NDP library..."
    cp -r "./NDP" "$ARDUINO_LIBRARIES_DIR/"
    
    print_status "success" "NDP library installed successfully"
}

# Function to install examples
install_examples() {
    print_status "step" "Installing Arduino examples..."
    
    local examples_dir="$HOME/Arduino/libraries/NiclaVoice-Examples"
    
    # Create examples directory
    mkdir -p "$examples_dir"
    
    # Copy examples
    print_status "info" "Copying examples to $examples_dir"
    cp -r "./examples"/* "$examples_dir/"
    
    print_status "success" "Examples installed successfully"
    print_status "info" "Examples location: $examples_dir"
}

# Function to verify installation
verify_installation() {
    print_status "step" "Verifying installation..."
    
    # Check NDP library
    if dir_exists "$ARDUINO_LIBRARIES_DIR/NDP"; then
        print_status "success" "NDP library found at $ARDUINO_LIBRARIES_DIR/NDP"
    else
        print_status "error" "NDP library not found"
        return 1
    fi
    
    # Check examples
    if dir_exists "$HOME/Arduino/libraries/NiclaVoice-Examples"; then
        print_status "success" "Examples found at $HOME/Arduino/libraries/NiclaVoice-Examples"
    else
        print_status "warning" "Examples not found"
    fi
    
    # Check key files
    if file_exists "$ARDUINO_LIBRARIES_DIR/NDP/src/NDP.cpp"; then
        print_status "success" "NDP.cpp found"
    else
        print_status "error" "NDP.cpp not found"
        return 1
    fi
    
    if file_exists "$ARDUINO_LIBRARIES_DIR/NDP/src/NDP.h"; then
        print_status "success" "NDP.h found"
    else
        print_status "error" "NDP.h not found"
        return 1
    fi
}

# Function to show usage instructions
show_usage_instructions() {
    print_status "step" "Usage Instructions"
    echo ""
    print_status "info" "📋 Next steps:"
    print_status "info" "1. Open Arduino IDE"
    print_status "info" "2. Select Board: Arduino Nicla Voice"
    print_status "info" "3. Open examples from: $HOME/Arduino/libraries/NiclaVoice-Examples"
    print_status "info" "4. Try: Record_and_stream_nodata/Record_and_stream_nodata.ino"
    echo ""
    print_status "info" "🔧 Key features of modified NDP library:"
    print_status "info" "- Logging disabled by default (prevents serial saturation)"
    print_status "info" "- New methods: enableLogging(), disableLogging(), isLoggingEnabled()"
    print_status "info" "- Overloaded begin() method: begin(fw1, enable_logging)"
    echo ""
    print_status "info" "📚 Examples included:"
    print_status "info" "- Record_and_stream_nodata: Basic audio capture"
    print_status "info" "- LoggingControlTest: Test logging control methods"
    print_status "info" "- LoggingDisabledTest: Verify logging is disabled by default"
    echo ""
    if [ -d "$BACKUP_DIR" ]; then
        print_status "info" "💾 Backup location: $BACKUP_DIR"
        print_status "info" "   (You can restore the original library from here if needed)"
    fi
}

# Main execution
main() {
    print_status "info" "Starting Arduino Nicla Voice libraries installation..."
    print_status "info" "Target directory: $ARDUINO_LIBRARIES_DIR"
    echo ""
    
    # Check if we're in the right directory
    if ! file_exists "./NDP/src/NDP.cpp"; then
        print_status "error" "NDP library not found in current directory"
        print_status "info" "Please run this script from the arduino-nicla-libraries directory"
        exit 1
    fi
    
    # Create backup
    create_backup
    
    # Install NDP library
    install_ndp_library
    
    # Install examples
    install_examples
    
    # Verify installation
    verify_installation
    
    # Show usage instructions
    show_usage_instructions
    
    echo ""
    print_status "success" "🎉 ARDUINO NICLA VOICE LIBRARIES INSTALLATION COMPLETE!"
    print_status "info" "📚 Documentation: See README.md for detailed information"
    print_status "info" "🔧 Verification: Check Arduino IDE for new examples"
}

# Run main function
main "$@"
