"use client";
import React, { useState } from "react";
import React from "react";

export const Plus = (props: React.SVGProps<SVGSVGElement>) => (
  <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2"
    strokeLinecap="round" strokeLinejoin="round" {...props}>
    <line x1="12" y1="5" x2="12" y2="19" />
    <line x1="5" y1="12" x2="19" y2="12" />
  </svg>
);

export const X = (props: React.SVGProps<SVGSVGElement>) => (
  <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2"
    strokeLinecap="round" strokeLinejoin="round" {...props}>
    <line x1="18" y1="6" x2="6" y2="18" />
    <line x1="6" y1="6" x2="18" y2="18" />
  </svg>
);

export const Edit2 = (props: React.SVGProps<SVGSVGElement>) => (
  <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2"
    strokeLinecap="round" strokeLinejoin="round" {...props}>
    <path d="M12 20h9" />
    <path d="M16.5 3.5a2.121 2.121 0 0 1 3 3L7 19l-4 1 1-4 12.5-12.5z" />
  </svg>
);

export const Eye = (props: React.SVGProps<SVGSVGElement>) => (
  <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2"
    strokeLinecap="round" strokeLinejoin="round" {...props}>
    <path d="M1 12s4-8 11-8 11 8 11 8-4 8-11 8-11-8-11-8z" />
    <circle cx="12" cy="12" r="3" />
  </svg>
);

export const Copy = (props: React.SVGProps<SVGSVGElement>) => (
  <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2"
    strokeLinecap="round" strokeLinejoin="round" {...props}>
    <rect x="9" y="9" width="13" height="13" rx="2" ry="2" />
    <path d="M5 15H4a2 2 0 0 1-2-2V4a2 2 0 0 1 2-2h9a2 2 0 0 1 2 2v1" />
  </svg>
);

export const Trash2 = (props: React.SVGProps<SVGSVGElement>) => (
  <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2"
    strokeLinecap="round" strokeLinejoin="round" {...props}>
    <polyline points="3 6 5 6 21 6" />
    <path d="M19 6v14a2 2 0 0 1-2 2H7a2 2 0 0 1-2-2V6m3 0V4a2 2 0 0 1 2-2h4a2 2 0 0 1 2 2v2" />
    <line x1="10" y1="11" x2="10" y2="17" />
    <line x1="14" y1="11" x2="14" y2="17" />
  </svg>
);

export const Settings = (props: React.SVGProps<SVGSVGElement>) => (
  <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2"
    strokeLinecap="round" strokeLinejoin="round" {...props}>
    <circle cx="12" cy="12" r="3" />
    <path d="M19.4 15a1.65 1.65 0 0 0 .33 1.82l.06.06a2 2 0 0 1-2.83 2.83l-.06-.06a1.65 1.65 0 0 0-1.82-.33 1.65 1.65 0 0 0-1 1.51V21a2 2 0 0 1-4 0v-.09a1.65 1.65 0 0 0-1-1.51 1.65 1.65 0 0 0-1.82.33l-.06.06a2 2 0 0 1-2.83-2.83l.06-.06a1.65 1.65 0 0 0 .33-1.82 1.65 1.65 0 0 0-1.51-1H3a2 2 0 0 1 0-4h.09c.7 0 1.31-.4 1.51-1a1.65 1.65 0 0 0-.33-1.82l-.06-.06a2 2 0 0 1 2.83-2.83l.06.06c.46.46 1.13.6 1.82.33h.09c.6-.2 1-.81 1-1.51V3a2 2 0 0 1 4 0v.09c0 .7.4 1.31 1 1.51h.09c.69.27 1.36.13 1.82-.33l.06-.06a2 2 0 0 1 2.83 2.83l-.06.06c-.46.46-.6 1.13-.33 1.82v.09c.2.6.81 1 1.51 1H21a2 2 0 0 1 0 4h-.09c-.7 0-1.31.4-1.51 1z" />
  </svg>
);

export const FileText = (props: React.SVGProps<SVGSVGElement>) => (
  <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2"
    strokeLinecap="round" strokeLinejoin="round" {...props}>
    <path d="M14 2H6a2 2 0 0 0-2 2v16a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2V8z" />
    <polyline points="14 2 14 8 20 8" />
    <line x1="16" y1="13" x2="8" y2="13" />
    <line x1="16" y1="17" x2="8" y2="17" />
    <line x1="10" y1="9" x2="8" y2="9" />
  </svg>
);

export const ChevronDown = (props: React.SVGProps<SVGSVGElement>) => (
  <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2"
    strokeLinecap="round" strokeLinejoin="round" {...props}>
    <polyline points="6 9 12 15 18 9" />
  </svg>
);

export const ChevronUp = (props: React.SVGProps<SVGSVGElement>) => (
  <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2"
    strokeLinecap="round" strokeLinejoin="round" {...props}>
    <polyline points="18 15 12 9 6 15" />
  </svg>
);


interface Question {
  id: number;
  type: string;
  title: string;
  description: string;
  required: boolean;
  options: string[];
  scaleMin: number;
  scaleMax: number;
  scaleMinLabel: string;
  scaleMaxLabel: string;
}

const FormBuilder = () => {
  const [formTitle, setFormTitle] = useState("無題のフォーム");
  const [formDescription, setFormDescription] = useState("");
  const [questions, setQuestions] = useState([]);
  const [isPreview, setIsPreview] = useState(false);
  const [responses, setResponses] = useState({});

  const questionTypes = [
    { value: "text", label: "記述式" },
    { value: "textarea", label: "段落" },
    { value: "radio", label: "ラジオボタン" },
    { value: "checkbox", label: "チェックボックス" },
    { value: "select", label: "プルダウン" },
    { value: "scale", label: "均等目盛" },
    { value: "date", label: "日付" },
    { value: "time", label: "時刻" },
  ];

  const addQuestion = () => {
    const newQuestion = {
      id: Date.now(),
      type: "text",
      title: "無題の質問",
      description: "",
      required: false,
      options: ["オプション 1"],
      scaleMin: 1,
      scaleMax: 5,
      scaleMinLabel: "",
      scaleMaxLabel: "",
    };
    setQuestions([...questions, newQuestion] as Question[]);
  };

  const updateQuestion = (id, field, value) => {
    setQuestions(
      questions.map((q) => (q.id === id ? { ...q, [field]: value } : q))
    );
  };

  const deleteQuestion = (id) => {
    setQuestions(questions.filter((q) => q.id !== id));
  };

  const duplicateQuestion = (id) => {
    const questionToDuplicate = questions.find((q) => q.id === id);
    const newQuestion = { ...questionToDuplicate, id: Date.now() };
    const index = questions.findIndex((q) => q.id === id);
    const newQuestions = [...questions];
    newQuestions.splice(index + 1, 0, newQuestion);
    setQuestions(newQuestions);
  };

  const addOption = (questionId) => {
    const question = questions.find((q) => q.id === questionId);
    const newOptions = [
      ...question.options,
      `オプション ${question.options.length + 1}`,
    ];
    updateQuestion(questionId, "options", newOptions);
  };

  const updateOption = (questionId, optionIndex, value) => {
    const question = questions.find((q) => q.id === questionId);
    const newOptions = [...question.options];
    newOptions[optionIndex] = value;
    updateQuestion(questionId, "options", newOptions);
  };

  const deleteOption = (questionId, optionIndex) => {
    const question = questions.find((q) => q.id === questionId);
    const newOptions = question.options.filter(
      (_, index) => index !== optionIndex
    );
    updateQuestion(questionId, "options", newOptions);
  };

  const handleResponseChange = (questionId, value) => {
    setResponses((prev) => ({
      ...prev,
      [questionId]: value,
    }));
  };

  const renderQuestionEditor = (question) => {
    return (
      <div
        key={question.id}
        className="bg-white rounded-lg shadow-sm border border-gray-200 p-6 mb-4"
      >
        <div className="flex items-start justify-between mb-4">
          <div className="flex-1">
            <input
              type="text"
              value={question.title}
              onChange={(e) =>
                updateQuestion(question.id, "title", e.target.value)
              }
              className="text-lg font-medium w-full border-b border-gray-300 focus:border-blue-500 focus:outline-none pb-2 mb-2"
              placeholder="質問"
            />
            <textarea
              value={question.description}
              onChange={(e) =>
                updateQuestion(question.id, "description", e.target.value)
              }
              className="w-full text-sm text-gray-600 border-b border-gray-200 focus:border-blue-500 focus:outline-none pb-2 resize-none"
              placeholder="説明（任意）"
              rows="1"
            />
          </div>
          <select
            value={question.type}
            onChange={(e) =>
              updateQuestion(question.id, "type", e.target.value)
            }
            className="ml-4 px-3 py-2 border border-gray-300 rounded-md focus:ring-2 focus:ring-blue-500 focus:border-transparent"
          >
            {questionTypes.map((type) => (
              <option key={type.value} value={type.value}>
                {type.label}
              </option>
            ))}
          </select>
        </div>

        <div className="mb-4">
          {question.type === "text" && (
            <input
              type="text"
              disabled
              placeholder="回答者の回答"
              className="w-full px-3 py-2 border-b border-gray-300 bg-gray-50 text-gray-500"
            />
          )}

          {question.type === "textarea" && (
            <textarea
              disabled
              placeholder="回答者の回答"
              className="w-full px-3 py-2 border border-gray-300 rounded-md bg-gray-50 text-gray-500 resize-none"
              rows="3"
            />
          )}

          {(question.type === "radio" || question.type === "checkbox") && (
            <div>
              {question.options.map((option, index) => (
                <div key={index} className="flex items-center mb-2">
                  <input type={question.type} disabled className="mr-3" />
                  <input
                    type="text"
                    value={option}
                    onChange={(e) =>
                      updateOption(question.id, index, e.target.value)
                    }
                    className="flex-1 border-b border-gray-300 focus:border-blue-500 focus:outline-none pb-1"
                  />
                  <button
                    onClick={() => deleteOption(question.id, index)}
                    className="ml-2 text-gray-400 hover:text-red-500"
                  >
                    <X size={16} />
                  </button>
                </div>
              ))}
              <button
                onClick={() => addOption(question.id)}
                className="flex items-center text-blue-600 hover:text-blue-800 mt-2"
              >
                <Plus size={16} className="mr-1" />
                オプションを追加
              </button>
            </div>
          )}

          {question.type === "select" && (
            <div>
              <select
                disabled
                className="w-full px-3 py-2 border border-gray-300 rounded-md bg-gray-50 text-gray-500"
              >
                <option>選択してください</option>
              </select>
              <div className="mt-2">
                {question.options.map((option, index) => (
                  <div key={index} className="flex items-center mb-2">
                    <span className="w-6 text-gray-500">{index + 1}.</span>
                    <input
                      type="text"
                      value={option}
                      onChange={(e) =>
                        updateOption(question.id, index, e.target.value)
                      }
                      className="flex-1 border-b border-gray-300 focus:border-blue-500 focus:outline-none pb-1"
                    />
                    <button
                      onClick={() => deleteOption(question.id, index)}
                      className="ml-2 text-gray-400 hover:text-red-500"
                    >
                      <X size={16} />
                    </button>
                  </div>
                ))}
                <button
                  onClick={() => addOption(question.id)}
                  className="flex items-center text-blue-600 hover:text-blue-800 mt-2"
                >
                  <Plus size={16} className="mr-1" />
                  オプションを追加
                </button>
              </div>
            </div>
          )}

          {question.type === "scale" && (
            <div>
              <div className="flex items-center mb-4">
                <input
                  type="number"
                  value={question.scaleMin}
                  onChange={(e) =>
                    updateQuestion(
                      question.id,
                      "scaleMin",
                      parseInt(e.target.value)
                    )
                  }
                  className="w-16 px-2 py-1 border border-gray-300 rounded text-center"
                  min="0"
                  max="1"
                />
                <span className="mx-2">から</span>
                <input
                  type="number"
                  value={question.scaleMax}
                  onChange={(e) =>
                    updateQuestion(
                      question.id,
                      "scaleMax",
                      parseInt(e.target.value)
                    )
                  }
                  className="w-16 px-2 py-1 border border-gray-300 rounded text-center"
                  min="2"
                  max="10"
                />
              </div>
              <div className="flex items-center justify-between">
                <input
                  type="text"
                  value={question.scaleMinLabel}
                  onChange={(e) =>
                    updateQuestion(question.id, "scaleMinLabel", e.target.value)
                  }
                  placeholder="最小値のラベル"
                  className="w-32 px-2 py-1 border-b border-gray-300 focus:border-blue-500 focus:outline-none"
                />
                <div className="flex space-x-2">
                  {Array.from(
                    { length: question.scaleMax - question.scaleMin + 1 },
                    (_, i) => (
                      <div key={i} className="flex flex-col items-center">
                        <input type="radio" disabled className="mb-1" />
                        <span className="text-sm">{question.scaleMin + i}</span>
                      </div>
                    )
                  )}
                </div>
                <input
                  type="text"
                  value={question.scaleMaxLabel}
                  onChange={(e) =>
                    updateQuestion(question.id, "scaleMaxLabel", e.target.value)
                  }
                  placeholder="最大値のラベル"
                  className="w-32 px-2 py-1 border-b border-gray-300 focus:border-blue-500 focus:outline-none"
                />
              </div>
            </div>
          )}

          {question.type === "date" && (
            <input
              type="date"
              disabled
              className="px-3 py-2 border border-gray-300 rounded-md bg-gray-50 text-gray-500"
            />
          )}

          {question.type === "time" && (
            <input
              type="time"
              disabled
              className="px-3 py-2 border border-gray-300 rounded-md bg-gray-50 text-gray-500"
            />
          )}
        </div>

        <div className="flex items-center justify-between pt-4 border-t border-gray-200">
          <label className="flex items-center text-sm text-gray-600">
            <input
              type="checkbox"
              checked={question.required}
              onChange={(e) =>
                updateQuestion(question.id, "required", e.target.checked)
              }
              className="mr-2"
            />
            必須
          </label>
          <div className="flex space-x-2">
            <button
              onClick={() => duplicateQuestion(question.id)}
              className="p-2 text-gray-500 hover:text-blue-600 hover:bg-blue-50 rounded-full"
              title="質問を複製"
            >
              <Copy size={16} />
            </button>
            <button
              onClick={() => deleteQuestion(question.id)}
              className="p-2 text-gray-500 hover:text-red-600 hover:bg-red-50 rounded-full"
              title="質問を削除"
            >
              <Trash2 size={16} />
            </button>
          </div>
        </div>
      </div>
    );
  };

  const renderQuestionPreview = (question) => {
    return (
      <div
        key={question.id}
        className="bg-white rounded-lg shadow-sm border border-gray-200 p-6 mb-4"
      >
        <div className="mb-4">
          <h3 className="text-lg font-medium mb-2">
            {question.title}
            {question.required && <span className="text-red-500 ml-1">*</span>}
          </h3>
          {question.description && (
            <p className="text-gray-600 text-sm mb-4">{question.description}</p>
          )}
        </div>

        <div>
          {question.type === "text" && (
            <input
              type="text"
              value={responses[question.id] || ""}
              onChange={(e) =>
                handleResponseChange(question.id, e.target.value)
              }
              className="w-full px-3 py-2 border-b border-gray-300 focus:border-blue-500 focus:outline-none"
              placeholder="回答を入力"
            />
          )}

          {question.type === "textarea" && (
            <textarea
              value={responses[question.id] || ""}
              onChange={(e) =>
                handleResponseChange(question.id, e.target.value)
              }
              className="w-full px-3 py-2 border border-gray-300 rounded-md focus:ring-2 focus:ring-blue-500 focus:border-transparent resize-none"
              placeholder="回答を入力"
              rows="3"
            />
          )}

          {question.type === "radio" && (
            <div>
              {question.options.map((option, index) => (
                <label
                  key={index}
                  className="flex items-center mb-2 cursor-pointer"
                >
                  <input
                    type="radio"
                    name={`question-${question.id}`}
                    value={option}
                    checked={responses[question.id] === option}
                    onChange={(e) =>
                      handleResponseChange(question.id, e.target.value)
                    }
                    className="mr-3"
                  />
                  <span>{option}</span>
                </label>
              ))}
            </div>
          )}

          {question.type === "checkbox" && (
            <div>
              {question.options.map((option, index) => (
                <label
                  key={index}
                  className="flex items-center mb-2 cursor-pointer"
                >
                  <input
                    type="checkbox"
                    value={option}
                    checked={(responses[question.id] || []).includes(option)}
                    onChange={(e) => {
                      const currentValues = responses[question.id] || [];
                      const newValues = e.target.checked
                        ? [...currentValues, option]
                        : currentValues.filter((v) => v !== option);
                      handleResponseChange(question.id, newValues);
                    }}
                    className="mr-3"
                  />
                  <span>{option}</span>
                </label>
              ))}
            </div>
          )}

          {question.type === "select" && (
            <select
              value={responses[question.id] || ""}
              onChange={(e) =>
                handleResponseChange(question.id, e.target.value)
              }
              className="w-full px-3 py-2 border border-gray-300 rounded-md focus:ring-2 focus:ring-blue-500 focus:border-transparent"
            >
              <option value="">選択してください</option>
              {question.options.map((option, index) => (
                <option key={index} value={option}>
                  {option}
                </option>
              ))}
            </select>
          )}

          {question.type === "scale" && (
            <div>
              <div className="flex items-center justify-between mb-2">
                <span className="text-sm text-gray-600">
                  {question.scaleMinLabel}
                </span>
                <span className="text-sm text-gray-600">
                  {question.scaleMaxLabel}
                </span>
              </div>
              <div className="flex justify-between">
                {Array.from(
                  { length: question.scaleMax - question.scaleMin + 1 },
                  (_, i) => (
                    <label
                      key={i}
                      className="flex flex-col items-center cursor-pointer"
                    >
                      <input
                        type="radio"
                        name={`question-${question.id}`}
                        value={question.scaleMin + i}
                        checked={
                          responses[question.id] == question.scaleMin + i
                        }
                        onChange={(e) =>
                          handleResponseChange(
                            question.id,
                            parseInt(e.target.value)
                          )
                        }
                        className="mb-1"
                      />
                      <span className="text-sm">{question.scaleMin + i}</span>
                    </label>
                  )
                )}
              </div>
            </div>
          )}

          {question.type === "date" && (
            <input
              type="date"
              value={responses[question.id] || ""}
              onChange={(e) =>
                handleResponseChange(question.id, e.target.value)
              }
              className="px-3 py-2 border border-gray-300 rounded-md focus:ring-2 focus:ring-blue-500 focus:border-transparent"
            />
          )}

          {question.type === "time" && (
            <input
              type="time"
              value={responses[question.id] || ""}
              onChange={(e) =>
                handleResponseChange(question.id, e.target.value)
              }
              className="px-3 py-2 border border-gray-300 rounded-md focus:ring-2 focus:ring-blue-500 focus:border-transparent"
            />
          )}
        </div>
      </div>
    );
  };

  return (
    <div className="min-h-screen bg-purple-50">
      <div className="max-w-4xl mx-auto p-4">
        {/* Header */}
        <div className="bg-white rounded-lg shadow-sm border border-gray-200 mb-6">
          <div className="px-6 py-4 border-b border-gray-200">
            <div className="flex items-center justify-between">
              <div className="flex items-center space-x-3">
                <div className="w-10 h-10 bg-purple-600 rounded-lg flex items-center justify-center">
                  <FileText className="w-6 h-6 text-white" />
                </div>
                <div>
                  <input
                    type="text"
                    value={formTitle}
                    onChange={(e) => setFormTitle(e.target.value)}
                    className="text-xl font-bold border-b border-gray-300 focus:border-purple-500 focus:outline-none pb-1"
                  />
                </div>
              </div>
              <div className="flex space-x-2">
                <button
                  onClick={() => setIsPreview(false)}
                  className={`px-4 py-2 rounded-md flex items-center space-x-2 ${
                    !isPreview
                      ? "bg-purple-600 text-white"
                      : "bg-gray-100 text-gray-600 hover:bg-gray-200"
                  }`}
                >
                  <Edit2 size={16} />
                  <span>編集</span>
                </button>
                <button
                  onClick={() => setIsPreview(true)}
                  className={`px-4 py-2 rounded-md flex items-center space-x-2 ${
                    isPreview
                      ? "bg-purple-600 text-white"
                      : "bg-gray-100 text-gray-600 hover:bg-gray-200"
                  }`}
                >
                  <Eye size={16} />
                  <span>プレビュー</span>
                </button>
              </div>
            </div>
          </div>

          <div className="px-6 py-4">
            <textarea
              value={formDescription}
              onChange={(e) => setFormDescription(e.target.value)}
              className="w-full text-gray-600 border-b border-gray-200 focus:border-purple-500 focus:outline-none pb-2 resize-none"
              placeholder="フォームの説明"
              rows="2"
            />
          </div>
        </div>

        {/* Form Content */}
        {!isPreview ? (
          <div>
            {/* Questions */}
            {questions.map(renderQuestionEditor)}

            {/* Add Question Button */}
            <div className="text-center">
              <button
                onClick={addQuestion}
                className="bg-purple-600 text-white px-6 py-3 rounded-full flex items-center space-x-2 hover:bg-purple-700 transition-colors mx-auto"
              >
                <Plus size={20} />
                <span>質問を追加</span>
              </button>
            </div>
          </div>
        ) : (
          <div>
            {/* Form Header */}
            <div className="bg-white rounded-lg shadow-sm border border-gray-200 p-6 mb-6">
              <h1 className="text-2xl font-bold text-purple-600 mb-2">
                {formTitle}
              </h1>
              {formDescription && (
                <p className="text-gray-600">{formDescription}</p>
              )}
            </div>

            {/* Questions Preview */}
            {questions.map(renderQuestionPreview)}

            {questions.length > 0 && (
              <div className="bg-white rounded-lg shadow-sm border border-gray-200 p-6 text-center">
                <button className="bg-purple-600 text-white px-8 py-3 rounded-md hover:bg-purple-700 transition-colors">
                  送信
                </button>
              </div>
            )}

            {questions.length === 0 && (
              <div className="bg-white rounded-lg shadow-sm border border-gray-200 p-12 text-center">
                <FileText className="w-16 h-16 text-gray-300 mx-auto mb-4" />
                <p className="text-gray-500 text-lg">まだ質問がありません</p>
                <p className="text-gray-400 text-sm mt-2">
                  編集モードで質問を追加してください
                </p>
              </div>
            )}
          </div>
        )}
      </div>
    </div>
  );
};

export default FormBuilder;
