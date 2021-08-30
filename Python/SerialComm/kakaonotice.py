import requests
import json

def pushMSG(message):
    url1 = 'https://kauth.kakao.com/oauth/token'
    rest_api_key = [YOUR_API_KEY]
    redirect_uri = 'https://example.com/oauth'
    authorize_code = [YOUR_AUTH_CODE]

    data = {
    'grant_type':'authorization_code',
    'client_id':rest_api_key,
    'redirect_uri':redirect_uri,
    'code': authorize_code,
    }

    response = requests.post(url1, data=data)
    tokens = response.json()
    print(tokens)



    #2.
    with open("kakao_code.json","w") as fp:
        json.dump(tokens, fp)

    with open("kakao_code.json","r") as fp:
        ts = json.load(fp)
        print(ts)
        print(ts["access_token"])

    url = "https://kapi.kakao.com/v2/api/talk/memo/default/send"

    # 사용자 토큰
    headers = {
        "Authorization": "Bearer " + ts["access_token"]
    }


    data = {
        "template_object" : json.dumps(
            { 
                "object_type" : "text",
                "text" : str(message),
                "link" : {"web_url" : "www.naver.com"}
            }
        )
    }

    response = requests.post(url, headers=headers, data=data)
    print(response.status_code)

    if response.json().get('result_code') == 0:
        print('메시지를 성공적으로 보냈습니다.')
    else:
        print('메시지를 성공적으로 보내지 못했습니다. 오류메시지 : ' + str(response.json()))

    return int(response.json().get('result_code'))