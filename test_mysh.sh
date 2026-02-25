# Test Script for mysh

# Ensure the executable exists
if [ ! -f "./mysh" ]; then
    echo "Error: ./mysh not found. Please run 'make' first."
    exit 1
fi

echo "========================================"
echo " Setting up test environment..."
echo "========================================"
mkdir -p test_dir
cd test_dir
echo "apple" > unsorted.txt
echo "zebra" >> unsorted.txt
echo "banana" >> unsorted.txt

# Create a file with no permissions to test error handling
touch no_perms.txt
chmod 000 no_perms.txt

echo "========================================"
echo " Running ./mysh..."
echo "========================================"

# Simulate user input into the shell
../mysh << 'EOF'

echo "[1] TESTING BASIC FUNCTIONALITY"
pwd
ls -la
echo "Multiple arguments test successful"
this_command_does_not_exist

echo ""
echo "[2] TESTING I/O REDIRECTION"
echo "Creating new file..." > output.txt
cat output.txt
echo "Appending to file..." >> output.txt
cat output.txt
echo "Counting lines in unsorted.txt:"
wc -l < unsorted.txt
echo "Combined redirection (sort < in > out):"
sort < unsorted.txt > sorted.txt
cat sorted.txt

echo ""
echo "[3] TESTING REDIRECTION ERRORS"
cat < does_not_exist.txt
echo "Should fail" > no_perms.txt

echo ""
echo "[4] TESTING BACKGROUND JOBS & REAPING"
sleep 2 &
sleep 3 &
echo "Shell is still responsive while sleeps run in background!"
echo "Waiting 4 seconds in foreground to let background jobs finish..."
sleep 4
echo "Pressing Enter (empty input) to trigger background check..."

echo "Background jobs should have reported as completed above."

echo ""
echo "[5] TESTING EDGE CASES"
echo "Spaces around operators"    >    spaced_out.txt
cat spaced_out.txt
echo "Output to dev null" > /dev/null

echo ""
echo "[6] TESTING BUILT-INS"
cd ..
pwd
exit
EOF

echo "========================================"
echo " Cleaning up test environment..."
echo "========================================"
cd ..
rm -rf test_dir

echo "Test complete! Scroll up to review your shell's output."