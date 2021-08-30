import tensorflow.keras
import numpy as np
import cv2

#레이블 가져오기
labels=[]
f=open("labels.txt", "r")
for x in f:
    labels.append(x.rstrip('\n'))
    label_count = len(labels)
f.close()

# e-04와 같은 scientific notation을 제거하고 싶을 때 사용하는 옵션
np.set_printoptions(suppress=True)

# Teachable Machine에서 학습시킨 모델 파일을 모델 파일을 컴파일을 다시 하지 않고, model 변수에 넣음
model = tensorflow.keras.models.load_model('keras_model.h5', compile=False)

# numpy를 이용해 이미지를 1차원, 높이 224pixel, 폭 224pixel, 색상 3채널(RGB)로 변환해서 data 변수에 넣음, 형식은 float32, 여기서는 data 변수를 만드는 의미로 쓰임
data = np.ndarray(shape=(1, 224, 224, 3), dtype=np.float32)

# OpenCV를 이용해 캠으로 들어오는 영상을 cap 변수에 넣음, '0'은 컴퓨터가 인식한 첫번째 카메라를 의미함
cap = cv2.VideoCapture(cv2.CAP_DSHOW+1)

print('Press "q", if you want to quit')

# while문 안의 내용을 계속 반복시킴. 캠에서 영상 프레임이 들어올 때마다 아래 프로그램을 실행함
while(True):

    ret, frame = cap.read()

    if ret:
        flip_frame = cv2.flip(frame, 1)


    h = flip_frame.shape[0]
    w = flip_frame.shape[1]


    crop_image = flip_frame[0:h, int((w-h)/2):int(w-((w-h)/2))]

    # 바이큐빅보간법(cv2.INTER_CUBIC, 이미지를 확대할 때 주로 사용)을 이용해 frame변수에 들어온 비디오 프레임의 사이즈를 224, 224로 다운사이징하여 image 변수에 넣음
    image = cv2.resize(crop_image, dsize=(224, 224), interpolation=cv2.INTER_CUBIC)

    # asarray메소드를 이용해 image에 들어있는 크기가 변형된 이미지를 numpy가 처리할 수 있는 배열로 만들어서 image_array 변수에 넣음
    image_array = np.asarray(image)

    # image_array에 들어있는 image의 변형된 배열을 정규화(normalized)하기 위해 수식을 적용함
    normalized_image_array = (image_array.astype(np.float32) / 127.0) - 1

    # 정규화된 배열을 data[0]에 넣음
    data[0] = normalized_image_array

    # 정규화된 배열값으로 정돈된 data를 Teachable Machine으로 학습시켜서 얻은 모델을 이용해 추론하고, 그 결과를 prediction 변수에 넣음
    prediction = model.predict(data)

    # 추론결과를 콘솔에 보여주기
    # print(prediction)

    # 글씨 넣기 준비
    font = cv2.FONT_HERSHEY_TRIPLEX
    fontScale = 1
    fontColor = (0,255,0)
    lineThickness = 1

    # 표기 문구 초기화
    scoreLabel = 0
    score = 0
    result = ''

    for x in range(0, label_count):
        #예측값 모니터링
        line=('%s=%0.0f' % (labels[x], int(round(prediction[0][x]*100)))) + "%"
        cv2.putText(crop_image, line, (10,(x+1)*35), font, fontScale, fontColor, lineThickness)

        # 가장 높은 예측 찾기
        if score < prediction[0][x]:
            scoreLabel = labels[x]
            score = prediction[0][x]
            result = str(scoreLabel) + " : " + str(score)
            print(result)

    # 최고 결과치 보여주기
    crop_image = cv2.putText(crop_image, result, (10, int(label_count+1)*35), font, 1, (0, 0, 255), 1, cv2.LINE_AA)

    # prediction의 첫번째 리스트값이 0.7을 넘으면 'a'를 utf-8 형태로 인코딩하여 시리얼 통신으로 송신함
    # 학습을 시킬 때는 반드시 디폴트 이미지를 학습시키는 것이 오류를 예방할 수 있는 지름길임
    if prediction[:, 0] > 0.7 :
    # 'a'를 utf-8 형식으로 인코딩 하여 send 변수에 넣음
        send = (str('a')+'\n').encode("utf-8")
    # send 변수에 들어있는 값을 시리얼통신으로 송신함
        #ser.write(send)
    # 송신이 되면 화면에 send 변수에 들어가 있는 값을 출력함
        print(send)

    # prediction의 두번째 리스트값이 0.7을 넘으면 'b'를 utf-8 형태로 인코딩하여 시리얼 통신으로 송신함
    if prediction[:, 1] > 0.7 :
        send = (str('b')+'\n').encode("utf-8")
        #ser.write(send)
        print(send)

    # prediction의 세번째 리스트값이 0.7을 넘으면 'c'를 utf-8 형태로 인코딩하여 시리얼 통신으로 송신함
    if prediction[:, 2] > 0.7 :
        send = (str('c')+'\n').encode("utf-8")
        #ser.write(send)
        print(send)

    # 줄바꿈
    print()

    # 원본이미지, 플립이미지, 크롭이미지, 추론용(축소)이미지 화면에 보여주기
    # cv2.imshow('frame',frame)
    # cv2.imshow('flip_frame',flip_frame)
    cv2.imshow('crop_image',crop_image)
    # cv2.imshow('image',image)

    # 키 입력을 기다림
    key = cv2.waitKey(1) & 0xFF

    # q 키를 눌렀다면 반복실행에서 종료함
    if key == ord("q"):
        print('Quit')
        break

    # 동작이 종료되면 비디오 프레임 캡쳐를 중단함
    cap.release()
    # 모든 창을 닫음
    cv2.destroyAllWindows()

