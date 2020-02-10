gcloud config set run/region us-central1

gcloud functions deploy relayCloudIot \
    --runtime nodejs8 \
    --stage-bucket=gs://home-automate-storage \
    --trigger-topic=water-temp-topic \
    --project=home-automation-hi-mod

openssl ecparam -genkey -name prime256v1 -noout -out ~/water-temp-device/ec_private.pem
openssl ec -in ~/water-temp-device/ec_private.pem -pubout -out ~/water-temp-device/ec_public.pem
openssl ec -in ~/water-temp-device/ec_private.pem -noout -text

gcloud iot devices create --region=us-central1 \
    --registry=home-automation-registry \
    --public-key path=~/water-temp-device/ec_public.pem,type=es256 \
    water-temp-device

gcloud beta functions logs read

openssl ecparam -genkey -name prime256v1 -noout -out ~/water-pump-device/ec_private.pem
openssl ec -in ~/water-pump-device/ec_private.pem -pubout -out ~/water-pump-device/ec_public.pem
openssl ec -in ~/water-pump-device/ec_private.pem -noout -text

gcloud iot devices create --region=us-central1 \
    --registry=home-automation-registry \
    --public-key path=~/water-pump-device/ec_public.pem,type=es256 \
    water-pump-device


# enable Endpoints for Google Cloud Functions
gcloud config set run/region us-central1

gcloud endpoints services deploy openapi-functions.yaml --project home-automation-hi-mod

chmod +x gcloud_build_image

./gcloud_build_image.sh -s endpointsservice-jjtyzc4ddq-uc.a.run.app -c 2020-01-20r0 -p home-automation-hi-mod

gcloud run deploy endpointsservice \
 --image="gcr.io/home-automation-hi-mod/endpoints-runtime-serverless:2020-01-20r0" \
 --allow-unauthenticated \
 --project=home-automation-hi-mod \
 --platform managed

gcloud beta functions add-iam-policy-binding activateWaterPump \
    --member "serviceAccount:170015086847-compute@developer.gserviceaccount.com" \
    --role "roles/cloudfunctions.invoker" \
    --project home-automation-hi-mod

gcloud beta functions add-iam-policy-binding actuateWaterPump \
    --member "serviceAccount:170015086847-compute@developer.gserviceaccount.com" \
    --role "roles/cloudfunctions.invoker" \
    --project home-automation-hi-mod