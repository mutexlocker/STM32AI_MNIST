# Hand written digits recognition on STM32F4

![alt text](https://i.imgur.com/0YPdsCz.png "Hand written digits recognition on STM32F4")
![alt text](https://codetolight.files.wordpress.com/2017/11/network.png?w=1108)

## Demo 
Link to demo video : https://www.youtube.com/watch?v=_YbzxUagpbw
### CNN MODEL
Simple CNN Model Is trained on MNIST dataset for handwriten digit recignition.
Tensorflow 2.0 with Keras is used for training and saving the model.
Link to the python code: 
https://github.com/nimaaghli/Mnist_TF2


### STMCUBEMX.AI
STMCUBEMX.AI is used to generate C code from the trained model.
Project is generated for SW4STM32 using STM32CUBEMX. SW4STM32 can be used to import and falsh the board or STM32AI_MNIST.ioc file can be loaded to CUBEMX to generate the code for diffrent softwares.  
## Hardware
This Demo runs on STM32F429I-DISC1 baord only, but can be easly modifed to be used on any STM32F4 board 




