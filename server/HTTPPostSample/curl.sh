curl -s -X POST http://127.0.0.1:8000/logs -H "Content-Type: application/json" -d '{"message":"hello from bash","user_id":1}'

curl -s -X POST "http://127.0.0.1:8000/logs" -H "Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJzdWIiOiJzYWJ1cm8iLCJleHAiOjE3NDkzNTEwNTB9.0UaUSys6kPapA6vMKK1XTVZfyMsQoRqi0N_g0KRuopI" -H "Content-Type: application/json" -d '{"message": "send after loggin", "user_id": 3}'
{
  "access_token": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJzdWIiOiJzYWJ1cm8iLCJleHAiOjE3NDkzNTAwOTN9.SS3qLfST_swpxRS_k8bgp78mrSwYSQC28NC1JkBQBIw",
  "token_type": "bearer"
}
{
  "access_token": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJzdWIiOiJzYWJ1cm8iLCJleHAiOjE3NDkzNTEwNTB9.0UaUSys6kPapA6vMKK1XTVZfyMsQoRqi0N_g0KRuopI",
  "token_type": "bearer"
}