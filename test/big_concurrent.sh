for i in {1..200}
do
  curl -H "Accept: application/xml" -H "Content-Type: application/xml" -X GET http://localhost:8888  && echo "HTTP Request: $i" &
done

