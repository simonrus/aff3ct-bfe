source ~/bin/anaconda.sh
conda activate py3

# Run Unittest
python -m unittest test_Aff3ctProtocol.py
# Run sample application
python aff3ct-client.py