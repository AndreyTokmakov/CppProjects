
docker-compose up -d clickhouse
docker-compose up -d mysql_db_service

docker-compose ps

docker-compose stop clickhouse
docker-compose stop mysql_db_service



docker-compose down
