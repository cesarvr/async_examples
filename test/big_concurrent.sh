for i in {1..10000}
do
  curl -H "Accept: application/xml" -H "Content-Type: application/xml" -X GET http://192.168.1.5:8888  && echo "HTTP Request: $i" &
done

