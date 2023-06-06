datakit_uuid=$(uuidgen)
printf '#define DATAKIT_APP_UUID "' > $1
printf $datakit_uuid >> $1
printf '"\n' >> $1

