
if [[ -z /etc/issue ]] ; then
    line=`sed "/^[ \t]*$/d" /etc/issue | head -1`
fi
#lower case
line=${line,,}
#
list_RPM_GNULinux=(suse mageia centos redhat)
list_APT_GNULinux=(debian ubuntu mint)
#
rpm=0
for os in ${list_RPM_GNULinux[*]}; do
    echo $line $os
    if [[ "$line" =~ "${$os}" ]]; then
	rpm=1
    fi
done
apt=0
for os in ${list_APT_GNULinux[*]}; do
    echo $line $os
    if [[ "$line" =~ "${$os}" ]]; then
	apt=1
    fi
done

if [[ $apt -eq 0 ]] && [[ $rpm -eq 0 ]] ; then
    echo "unknown system, please report !"
fi
if [[ $apt -eq 1 ]] ; then
    aptget4gdl.sh
fi
if [[ $rpm -eq 1 ]] ; then
    rpm4gdl.sh
fi
