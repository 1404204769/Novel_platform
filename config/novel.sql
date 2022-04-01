/*
 Navicat MySQL Data Transfer

 Source Server         : drogon_web
 Source Server Type    : MariaDB
 Source Server Version : 100334
 Source Host           : 192.168.174.132:3306
 Source Schema         : novel

 Target Server Type    : MariaDB
 Target Server Version : 100334
 File Encoding         : 65001

 Date: 01/04/2022 23:47:12
*/

SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
-- Table structure for Idea
-- ----------------------------
DROP TABLE IF EXISTS `Idea`;
CREATE TABLE `Idea`  (
  `Idea_ID` int(10) NOT NULL AUTO_INCREMENT COMMENT '反馈意见ID',
  `User_ID` int(10) NOT NULL COMMENT '用户ID',
  `Type` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL COMMENT '类型',
  `Memo` text CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL COMMENT '详细参数',
  `Status` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL COMMENT '状态',
  `Create` timestamp(0) NOT NULL DEFAULT current_timestamp() COMMENT '创建时间',
  `Update` timestamp(0) NOT NULL DEFAULT current_timestamp() ON UPDATE CURRENT_TIMESTAMP COMMENT '更新时间',
  PRIMARY KEY (`Idea_ID`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for action
-- ----------------------------
DROP TABLE IF EXISTS `action`;
CREATE TABLE `action`  (
  `Action_ID` int(10) NOT NULL AUTO_INCREMENT COMMENT '行为编号',
  `User_ID` int(10) NOT NULL COMMENT '用户编号',
  `Type` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL COMMENT '类型',
  `Memo` text CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL COMMENT '详细参数',
  PRIMARY KEY (`Action_ID`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for agree
-- ----------------------------
DROP TABLE IF EXISTS `agree`;
CREATE TABLE `agree`  (
  `Agree_ID` int(10) NOT NULL AUTO_INCREMENT COMMENT '点赞ID',
  `User_ID` int(10) NOT NULL COMMENT '用户ID',
  `Note_ID` int(10) NOT NULL COMMENT '帖子ID',
  `Comment_ID` int(10) NOT NULL COMMENT '评论ID',
  `Status` tinyint(1) NOT NULL COMMENT '是否点赞',
  `Time` timestamp(0) NOT NULL DEFAULT current_timestamp() ON UPDATE CURRENT_TIMESTAMP COMMENT '点赞时间',
  PRIMARY KEY (`Agree_ID`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for book
-- ----------------------------
DROP TABLE IF EXISTS `book`;
CREATE TABLE `book`  (
  `Book_ID` int(10) NOT NULL AUTO_INCREMENT COMMENT '图书编号',
  `Name` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL COMMENT '书名',
  `Status` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL COMMENT '状态',
  `Synopsis` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL COMMENT '提要',
  `Publisher` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL COMMENT '出版方',
  `Author` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL COMMENT '作者',
  `Create_time` timestamp(0) NOT NULL DEFAULT current_timestamp() COMMENT '创建时间',
  `Update_time` timestamp(0) NOT NULL DEFAULT current_timestamp() ON UPDATE CURRENT_TIMESTAMP COMMENT '更新时间',
  `Memo` text CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL COMMENT '详细参数',
  PRIMARY KEY (`Book_ID`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 10 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for chapter
-- ----------------------------
DROP TABLE IF EXISTS `chapter`;
CREATE TABLE `chapter`  (
  `Chapter_D` int(10) NOT NULL AUTO_INCREMENT COMMENT '章节编号',
  `Book_ID` int(10) NOT NULL COMMENT '图书编号',
  `Order_ID` int(10) NOT NULL COMMENT '顺序编号',
  `Valid` tinyint(1) NOT NULL COMMENT '是否有效',
  `Content` text CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL COMMENT '章节内容',
  `Remarks` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL COMMENT '备注',
  `User_ID` int(10) NOT NULL COMMENT '贡献者ID',
  `Title` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL COMMENT '章节标题',
  PRIMARY KEY (`Chapter_D`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for comment
-- ----------------------------
DROP TABLE IF EXISTS `comment`;
CREATE TABLE `comment`  (
  `Comment_ID` int(10) NOT NULL AUTO_INCREMENT COMMENT '评论ID',
  `Note_ID` int(10) NOT NULL COMMENT '帖子ID',
  `Floor_ID` int(10) NOT NULL COMMENT '楼层编号',
  `User_ID` int(10) NOT NULL COMMENT '用户ID',
  `Content` text CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL COMMENT '评论内容',
  `Create` timestamp(0) NOT NULL DEFAULT current_timestamp() COMMENT '评论时间',
  `Reply_Floor_ID` int(10) NOT NULL COMMENT '评论上层ID',
  PRIMARY KEY (`Comment_ID`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for note
-- ----------------------------
DROP TABLE IF EXISTS `note`;
CREATE TABLE `note`  (
  `Note_ID` int(10) NOT NULL AUTO_INCREMENT COMMENT '帖子ID',
  `Title` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL COMMENT '标题',
  `Content` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL COMMENT '内容',
  `Create` timestamp(0) NOT NULL DEFAULT current_timestamp() COMMENT '发布时间',
  `Read_Count` int(255) NOT NULL COMMENT '阅读量',
  `User_ID` int(10) NOT NULL COMMENT '用户ID',
  `Status` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL COMMENT '状态',
  `Type` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL COMMENT '类型',
  PRIMARY KEY (`Note_ID`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for upload
-- ----------------------------
DROP TABLE IF EXISTS `upload`;
CREATE TABLE `upload`  (
  `Upload_ID` int(10) NOT NULL AUTO_INCREMENT COMMENT '上传ID',
  `User_ID` int(10) NOT NULL COMMENT '用户ID',
  `Book_ID` int(10) NOT NULL COMMENT '图书ID',
  `Content` text CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL COMMENT '上传内容',
  `Status` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL COMMENT '上传状态/处理结果',
  `Time` timestamp(0) NOT NULL DEFAULT current_timestamp() COMMENT '上传时间',
  `Processor` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NULL DEFAULT NULL COMMENT '处理人',
  `IsManage` tinyint(1) NOT NULL COMMENT '是否处理',
  PRIMARY KEY (`Upload_ID`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 12 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for user
-- ----------------------------
DROP TABLE IF EXISTS `user`;
CREATE TABLE `user`  (
  `User_ID` int(10) NOT NULL COMMENT '用户编号',
  `Name` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL COMMENT '用户名',
  `Password` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL COMMENT '密码',
  `Sex` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL COMMENT '性别',
  `Integral` int(10) NOT NULL COMMENT '积分',
  `Level` int(10) NOT NULL COMMENT '等级',
  `Power` int(10) NOT NULL COMMENT '权限',
  `Status` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL COMMENT '状态',
  `Total_Integral` int(10) NOT NULL COMMENT '总积分',
  PRIMARY KEY (`User_ID`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci ROW_FORMAT = Dynamic;

SET FOREIGN_KEY_CHECKS = 1;
