#!bin/bash
echo "Recompiling source"

bash /clear.sh
make
clear

printf "Running tests\n\n"

success=0
fail=0

success_names=()
fail_names=()

function run_test() {
    test_name=${1##*/}
    echo ${test_name,,}
    out=$(./main.exe --file $1)
    result=$?
        
    printf "$out\n"
    printf "$result\n"
    printf "\n\n"

    if [[ ${result} == 0 ]]; then
        local_success=$((local_success+1))
        success=$((success+1))
        success_names+=(${test_name})
    else
        local_fail=$((local_success+1))
        fail=$((fail+1))
        fail_names+=(${test_name})
    fi
} 

for folder in tests/*; do
    folder_name=${folder#*/}
    printf "${folder_name^^}\n\n"
    
    local_success=0
    local_fail=0

    for subfolder in $folder/*; do
        if [ -d $subfolder ]; then
            for subsubfolder in $subfolder/*; do
                run_test $subsubfolder;
            done
        else
            run_test $subfolder;
        fi
    done

    echo "Success: ${local_success}"
    printf "Fail: ${local_fail}\n\n\n"
done

printf "All tests performed\n\n"
echo "Success: ${success}"
printf "Fail: ${fail}\n\n"

failed_count=${#fail_names[@]}

if [[ ${failed_count} == 0 ]]; then
    printf "All tests successful\n\n"
else
    for i in "{${fail_names[@]}}"; do
        echo "$i"
    done
fi

read