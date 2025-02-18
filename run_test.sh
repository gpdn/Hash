#!bin/bash
echo "Recompiling source"

bash /clear.sh
make
clear

printf "Running tests\n\n"

success=0
fail=0

for folder in tests/*; do
    folder_name=${folder#*/}
    printf "${folder_name^^}\n\n"
    
    local_success=0
    local_fail=0

    for subfolder in $folder/*; do
        test_name=${subfolder##*/}
        echo ${test_name,,}
        out=$(./main.exe --file $subfolder)
        result=$?
        
        printf "$out\n"
        printf "$result\n"
        printf "\n\n"

        if [[ ${result} == 0 ]]; then
            local_success=$((local_success+1))
            success=$((success+1))
            continue
        fi

        local_fail=$((local_success+1))
        fail=$((fail+1))
    done

    echo "Success: ${local_success}"
    printf "Fail: ${local_fail}\n\n\n"
done

printf "All test performed\n\n"
echo "Success: ${success}"
echo "Fail: ${fail}"

read