for i in `seq 1 50000`
do
  curl -H "Accept: application/xml" -H "Content-Type: application/xml" -X GET http://localhost:8888&
done
