#!/bin/bash
source .venv/bin/activate
python3 -m build
pip install --editable .
