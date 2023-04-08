# O2AT - Second edition - Machine Learning primer


## Neural network regression

The notebook on neural network regression is meant as complementary material to the presentation given in the O2 analysis tutorial (24.04.23 - 28.04.23). It illustrates the basics of neural network regression and shows how to import, train and export data and neural network models using pytorch.

### Table of contents
* [Information](#general-info)
* [Classes](#classes)
* [Example data](#data)

### Information

The neural network code is straight forward and doesn't require other libraries than the ones state in the first markdown cell in the notebook. No further modifications are required although they are highly recommended! This notebook is meant to be played around with or for testing purposes and should not be seen as a general purpose neural network class. Furthermore, no guarantee is given for the correctness of all the formulas statet in the notebook.

### Classes

In the "NeuralNetworkRegression/classes" folder you can find a class for importing and exporting root files from and to python numpy arrays. All other classes that are required are introduced in the notebook.

### Example data

For the tutorial itself, sample data is either generated directly in the notebook (Section 3, Regression in 1D) or by using prepared data from the folder "NeuralNetworkRegression/LHC22q". Due to time-constraints and computing resources, neither the data-generation of actual ALICE data nor the neural network training on it can be performed within the tutorial. Please use the pretrained model instead ("NeuralNetworkRegression/LHC22q/net.onnx").



## BDT