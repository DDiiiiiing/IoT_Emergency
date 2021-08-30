import tensorflow.keras
import numpy as np
import cv2

def img_normalize(crop_image):
    # 바이큐빅보간법(cv2.INTER_CUBIC, 이미지를 확대할 때 주로 사용)을 이용해 frame변수에 들어온 비디오 프레임의 사이즈를 224, 224로 다운사이징하여 image 변수에 넣음
    resize_image = cv2.resize(crop_image, dsize=(224, 224), interpolation=cv2.INTER_CUBIC)

    # asarray메소드를 이용해 image에 들어있는 크기가 변형된 이미지를 numpy가 처리할 수 있는 배열로 만들어서 image_array 변수에 넣음
    image_array = np.asarray(resize_image)

    # image_array에 들어있는 image의 변형된 배열을 정규화(normalized)하기 위해 수식을 적용함
    normalized_image_array = (image_array.astype(np.float32) / 127.0) - 1

    return normalized_image_array

def img_crop(image):
    flip_image = cv2.flip(image, 1)

    h = flip_image.shape[0]
    w = flip_image.shape[1]

    crop_image = flip_image[0:h, int((w-h)/2):int(w-((w-h)/2))]

    return crop_image