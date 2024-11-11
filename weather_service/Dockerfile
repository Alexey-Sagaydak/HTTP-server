FROM python:3.9-slim

WORKDIR /app
COPY weather_service.py /app/
RUN pip install flask requests

EXPOSE 5000
CMD ["python", "weather_service.py"]
