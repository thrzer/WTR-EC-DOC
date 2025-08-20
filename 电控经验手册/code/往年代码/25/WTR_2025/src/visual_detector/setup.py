from setuptools import find_packages, setup

package_name = 'visual_detector'

setup(
    name=package_name,
    version='0.0.0',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='mio',
    maintainer_email='kurihara.mio1026@gmail.com',
    description='TODO: Basket_detect and tag_detect package',
    license='MIT',
    # tests_require=['pytest'],
    entry_points={
        'console_scripts': [
            'basket_detector = visual_detector.basket_detector:main',
        ],
    },
)
