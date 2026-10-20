from setuptools import find_packages, setup

with open("README.md", "r", encoding="utf-8") as fh:
    long_description = fh.read()

setup(
    name="wordkit",
    version="1.0.0",
    author="Ragul Ramachandran",
    author_email="rragulramachandran@gmail.com",
    description="A modular Python package for text processing and metric analytics",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/rragulramachandran/College/tree/main/FreeOpenSourceSoftware/24BCS174/Assignment_2_TextTools",
    packages=find_packages(),
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: MIT License",
        "Operating System :: OS Independent",
    ],
    python_requires=">=3.8",
)
